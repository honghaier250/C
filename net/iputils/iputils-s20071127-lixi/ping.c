
/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Mike Muuss.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the University of
 *    California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef lint
char copyright[] = "@(#) Copyright (c) 1989 The Regents of the University of California.\n\
                    All rights reserved.\n";
#endif                            /* not lint */

/*
 *            P I N G . C
 *
 * Using the InterNet Control Message Protocol (ICMP) "ECHO" facility,
 * measure round-trip-delays and packet loss across network paths.
 *
 * Author -
 *    Mike Muuss
 *    U. S. Army Ballistic Research Laboratory
 *    December, 1983
 *
 * Status -
 *    Public Domain.  Distribution Unlimited.
 * Bugs -
 *    More statistics could always be gathered.
 *    This program has to run SUID to ROOT to access the ICMP socket.
 */

#include "ping_common.h"

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#ifndef ICMP_FILTER
#define ICMP_FILTER    1
struct icmp_filter
{
    __u32 data;
};
#endif


#define    MAXIPLEN    60
#define    MAXICMPLEN    76
#define    NROUTES        9            /* number of record route slots */
#define TOS_MAX        255            /* 8-bit TOS field */


static int ts_type;

static int nroute = 0;

static __u32 route[10];



struct sockaddr_in whereto;        /* who to ping */

int optlen = 0;

int settos = 0;                    /* Set TOS, Precendence or other QOS options */

int icmp_sock;                    /* socket file descriptor */

u_char outpack[0x10000];

int maxpacket = sizeof (outpack);

static int broadcast_pings = 0;

static char *pr_addr (__u32);

static void pr_options (unsigned char *cp, int hlen);

static void pr_iph (struct iphdr *ip);

static void usage (void) __attribute__ ((noreturn));

static u_short in_cksum (const u_short * addr, int len, u_short salt);

static void pr_icmph (__u8 type, __u8 code, __u32 info, struct icmphdr *icp);

static int parsetos (char *str);

static struct
{
    struct cmsghdr cm;
    struct in_pktinfo ipi;
} cmsg =
{
    {
        sizeof (struct cmsghdr) + sizeof (struct in_pktinfo), SOL_IP, IP_PKTINFO},
    {
        0,}};

int cmsg_len;

struct sockaddr_in source;

/*
   struct sockaddr_in {
   sa_family_t    sin_family;  //地址种类 AF_INET代表IPV4
   u_int16_t      sin_port;   //端口
   struct in_addr sin_addr;   //地址
   }

   struct in_addr {
   u_int32_t      s_addr;     // 地址是按照网络字节顺序排列的
   };
*/
char *device;

int pmtudisc = -1;


int main (int argc, char **argv)
{

    /*
       argc等于输入字符串数的大小
       argv指向第一个字符串，一般为ping
       lixiprintf("%d\n",argc);
       lixiprintf("%s\n",argv[0]);
    */

    struct hostent *hp;

    int ch, hold, packlen;

    int socket_errno;

    u_char *packet;

    char *target, hnamebuf[MAXHOSTNAMELEN];

    char rspace[3 + 4 * NROUTES + 1];    /* record route space */

    icmp_sock = socket (AF_INET, SOCK_RAW, IPPROTO_ICMP);

    /*
       报文类型为ICMP报文
    */
    socket_errno = errno;

    /*
       SOCK_RAW：Provides raw network protocol access.
    */
    uid = getuid ();

    /*
       只有root用户的uid是0。
    */
    if (setuid (uid))
    {

        /*
           If the process has appropriate privileges, setuid() shall set the real user ID, effective user ID, and the saved set-user-ID of the calling process to uid.

           If the process does not have appropriate privileges, but uid is equal to the real user ID or the saved set-user-ID, setuid() shall set the effective user ID to uid; the real user ID and saved set-user-ID shall remain unchanged.

           The setuid() function shall not affect the supplementary group list in any way.
        */
        perror ("ping: setuid");
        exit (-1);
    }

    source.sin_family = AF_INET;

    /*
       源地址是IPv4
     */
    preload = 1;
    while ((ch = getopt (argc, argv, COMMON_OPTSTR "bRT:")) != EOF)
    {

        /*
           COMMON_OPTSTR中定义了许多bRT:之外的字符串，两者和在一起，作为getopt的参数
        */

        /*
            getopt()函数

            #include <unistd.h>

            int getopt(int argc,char *const *argv,const char *optstring);
            extern char *optarg;//  Ptr to option argument
            extern int optind;//  argv[] index
            extern int opterr;//  Controls error processing
            1.指针 optarg 指向[正在处理]的的[选项的参数]
            比如getopt()在处理选项-Wall或-W all
            [调用返回后]optarg指向[C字符串]all
            2.变量optind[初始化值为1]。
            getopt()将optind的数值置为
            将要使用的下一个argv[]数组的下标。
            所以当[选项]处理完后，optind为第一个[参数]在argv[]中的下标。

            <这个变量的作用很大，可以用来区分[选项]和[参数]，这两个概念一定要区分开来>
            <实际上，getopt()用来处理[选项]和[选项的参数]，对于[参数]（独立的参数，不是选项中带的），就要等[选项]处理完用optind来处理了>
            <从这里你就可以看出为什么敲命令行时要先敲[选项]，再敲[参数]了>

            3.变量opterr[初始化为1(表示true)]，并作为getopt()函数的一个[输入]。
            当opterr为true并且碰到一个不认识字符时
            向stderr输出一个消息指出不认识的选项。
            当opterr为false并且碰到一个不认识字符时
            禁止输出错误消息。
            4.返回值 当处理[选项]时，返回处理[选项]的单个字符
            当处理完[选项]时，返回EOF。接下来可以利用optind处理[参数]
            5.函数原型中的参数 argc,argv意义就很明显了。optstring用来给出你需要处理的选项

         */
        switch (ch)
        {
            case 'b':
                broadcast_pings = 1;

                /*
                   允许ping一个广播地址
                */
                break;
            case 'Q':

                /*
                   -Q参数用来设置服务质量（Quality of Service ）
                   例如最小开销、 可靠性、吞吐量、低延迟
                   IP协议有一个8bit的DS区分服务（以前叫服务类型）。前三位是优先（precedence）字段（在目前，优先字段并未被大家使用），接着4bit是TOS位，最后1bit好像没有使用。
                   4比特TOS位的意义分别为D（最小时延）、T（最大吞吐量）、R（最高可靠性）、C（最小代价）
                   要设置TOS位为对应意义，可以设置-Q <tos>分别为0x10，0x08，0x04，0x02
                */
                settos = parsetos (optarg);
                if (settos && (setsockopt (icmp_sock, IPPROTO_IP, IP_TOS, (char *) &settos, sizeof (int)) < 0))
                {

                    /*
                       设置IP数据报头部的IP_TOS
                       IP_TOS:Set or receive the Type-Of-Service (TOS) field that is sent with every IP packet originating from this socket.
                    */
                    perror ("ping: error setting QOS sockopts");
                    exit (2);
                }
                break;
            case 'R':
                if (options & F_TIMESTAMP)
                {
                    fprintf (stderr, "Only one of -T or -R may be used\n");
                    exit (2);
                }
                options |= F_RROUTE;
                break;
            case 'T':
                if (options & F_RROUTE)
                {
                    fprintf (stderr, "Only one of -T or -R may be used\n");
                    exit (2);
                }
                options |= F_TIMESTAMP;
                if (strcmp (optarg, "tsonly") == 0)
                    ts_type = IPOPT_TS_TSONLY;
                else if (strcmp (optarg, "tsandaddr") == 0)
                    ts_type = IPOPT_TS_TSANDADDR;
                else if (strcmp (optarg, "tsprespec") == 0)
                    ts_type = IPOPT_TS_PRESPEC;
                else
                {
                    fprintf (stderr, "Invalid timestamp type\n");
                    exit (2);
                }
                break;
            case 'I':
                {

                    /*
                       这个参数用来设置源主机的接口地址或者设备名
                       可能使用在主机有多个网卡等情况下
                    */
#if 0
                    char dummy;

                    int i1, i2, i3, i4;

                    if (sscanf (optarg, "%u.%u.%u.%u%c", &i1, &i2, &i3, &i4, &dummy) == 4)
                    {
                        __u8 *ptr;

                        ptr = (__u8 *) & source.sin_addr;
                        ptr[0] = i1;
                        ptr[1] = i2;
                        ptr[2] = i3;
                        ptr[3] = i4;
                        options |= F_STRICTSOURCE;
                    }
                    else
                    {
                        device = optarg;
                    }
#else
                    if (inet_pton (AF_INET, optarg, &source.sin_addr) > 0)

                        /*
                           设置源地址
                           AF_INET参数表示使用IPv4地址
                           若使用AF_INET6则表示使用IPv6地址
                        */

                        /*
                           如果能够正确的转换，则函数返回正数值
                        */
                        options |= F_STRICTSOURCE;
                    else

                        /*
                           如果不能正确转换，则意味着这个参数不是地址，如210.45.74.25，而是设备名，如eth0
                        */

                        device = optarg;
#endif
                    break;
                }
            case 'M':
                if (strcmp (optarg, "do") == 0)
                    pmtudisc = IP_PMTUDISC_DO;
                else if (strcmp (optarg, "dont") == 0)
                    pmtudisc = IP_PMTUDISC_DONT;
                else if (strcmp (optarg, "want") == 0)
                    pmtudisc = IP_PMTUDISC_WANT;
                else
                {
                    fprintf (stderr, "ping: wrong value for -M: do, dont, want are valid ones.\n");
                    exit (2);
                }
                break;
            case 'V':
                printf ("ping utility, iputils-ss%s\n", SNAPSHOT);
                exit (0);

                /*
                   打印版本信息然后退出
                */
                COMMON_OPTIONS

                    /*
                       定义在ping_common.h中
                       处理和ping6共有的参数
                    */
                    common_options (ch);
                break;
            default:
                usage ();

                /*
                   使用不支持的参数，报错
                */
        }
    }
    argc -= optind;
    argv += optind;

    /*
       getopt()将optind的数值置为将要使用的下一个argv[]数组的下标。
       在退出循环之后，optind将所有的
    */
    if (argc == 0)
        usage ();

    /*
       没有给出地址，报错
    */
    if (argc > 1)
    {

        /*
           给出了除地址外的的其他参数
        */
        if (options & F_RROUTE)
        {

            /*
               记录路径时，IP头选项字段只能用来存储路由信息
            */
            usage ();
        }
        else if (options & F_TIMESTAMP)
        {
            if (ts_type != IPOPT_TS_PRESPEC)
                usage ();
            if (argc > 5)

                /*
                   这种情况只有可能发生在：
                   -T tsprespec [host1 [host2 [host3 [host4]]]]
                   且不IP地址数和时间戳数不多于4
                */
                usage ();
        }
        else
        {
            if (argc > 10)

                /*
                   route[]数组最多可以存储10个地址
                */
            {
                usage ();
            }
            options |= F_SOURCEROUTE;
        }
    }
    while (argc > 0)
    {

        /*
           ping主机，主机数还有argc个
        */
        target = *argv;

        /*
           target是当前ping的目标主机
        */

        memset ((char *) &whereto, 0, sizeof (whereto));
        whereto.sin_family = AF_INET;
        if (inet_aton (target, &whereto.sin_addr) == 1)
        {

            /*
               inet_aton将target中以数字和点作为形式的因特网地址转换为struct in_addr
            */
            hostname = target;
            if (argc == 1)
                options |= F_NUMERIC;

            /*
               设置options的F_NUMERIC
            */
        }
        else
        {
            hp = gethostbyname (target);

            /*
               target不是ipv4标准的数字和点组成的地址，则通过gethostbyname()函数查询DNS获得地址
               either a host name, or an IPv4 address in standard dot notation, or an IPv6 address in colon (and possibly dot) notation
            */
            if (!hp)
            {

                /*无法获得正确的ip地址*/
                fprintf (stderr, "ping: unknown host %s\n", target);
                exit (2);
            }
            memcpy (&whereto.sin_addr, hp->h_addr, 4);

            /*
               从hp->h_addr复制4byte到&whereto.sin_addr（hostent结构）
               hp中不仅仅有地址信息还有很多
               struct sockaddr_in whereto
               hostent结构定义在<netdb.h>中
            */
            strncpy (hnamebuf, hp->h_name, sizeof (hnamebuf) - 1);

            /*
               h_name是主机的官方名字
            */
            hnamebuf[sizeof (hnamebuf) - 1] = 0;

            /*
               将字符串的最后一个字符设为NULL
            */
            hostname = hnamebuf;

            /*
               将可能不规范的主机名字改为为官方名字
               运行到退出循环hostname时，存储的是最后一个主机名字
               这就意味这ping的是最后一个出现的主机
            */
        }
        if (argc > 1)
            route[nroute++] = whereto.sin_addr.s_addr;

        /*
           将主机地址存储起来
        */

        /*
           route中保存了所有的目的主机地址
           nroute是可以ping的主机数-1
        */
        argc--;
        argv++;
        /*处理下一个 */
    }

    /*处理主机地址完毕
      printf("nroute:%d\n",nroute);
    */
    if (source.sin_addr.s_addr == 0)
    {

        /*
           struct sockaddr_in source;
           没有设置源主机地址
           源主机地址只可能在-I <hostName>参数中设置
        */
        socklen_t alen;

        struct sockaddr_in dst = whereto;

        int probe_fd = socket (AF_INET, SOCK_DGRAM, 0);

        /*
           取得一个UDP类型的socket描述符
           这个探针用来试探目的主机的基本情况
           SOCK_DGRAM表明：无连接的、不可靠的信息，信息的最大长度固定

        */

        if (probe_fd < 0)
        {

            /*
               没有申请成功？，出错
            */
            perror ("socket");
            exit (2);
        }
        if (device)
        {

            /*
               device即网络设备的名字
               只有在有-I选项时device被设置;
            */

            struct ifreq ifr;

            memset (&ifr, 0, sizeof (ifr));
            strncpy (ifr.ifr_name, device, IFNAMSIZ - 1);
            if (setsockopt (probe_fd, SOL_SOCKET, SO_BINDTODEVICE, device, strlen (device) + 1) == -1)
            {

                /*
                   设置socket选项
                   如果没有网络设备device，则不会成功
                   SO_BINDTODEVICE:Bind this socket to a particular device like "eth0", as specified in the passed interface name.
                */
                if (IN_MULTICAST (ntohl (dst.sin_addr.s_addr)))
                {

                    /*
                       测试是不是一个多播地址
                    */
                    struct ip_mreqn imr;

                    if (ioctl (probe_fd, SIOCGIFINDEX, &ifr) < 0)
                    {

                        /*
                           ioctl是设备驱动程序中对设备的I/O通道进行管理的函数
                        */
                        fprintf (stderr, "ping: unknown iface %s\n", device);
                        exit (2);
                    }
                    memset (&imr, 0, sizeof (imr));
                    imr.imr_ifindex = ifr.ifr_ifindex;
                    if (setsockopt (probe_fd, SOL_IP, IP_MULTICAST_IF, &imr, sizeof (imr)) == -1)
                    {

                        /*
                           组播
                           IP_MULTICAST_IF:Set the local device for a multicast socket.
                        */
                        perror ("ping: IP_MULTICAST_IF");
                        exit (2);
                    }
                }
            }
        }

        if (settos && setsockopt (probe_fd, IPPROTO_IP, IP_TOS, (char *) &settos, sizeof (int)) < 0)
            perror ("Warning: error setting QOS sockopts");

        /*
           如果设置了服务质量参数
           则对socket选项进行相应设置
        */
        dst.sin_port = htons (1025);

        /*
           htons将主机字节顺序转换为网络的字节顺序
           因为主机有不同的大小端，而网络是一致的
           与上面ntohl的效果相对

           netstat命令查看端口号

           tcp/udp 的端口分为三类：熟知端口，注册端口，和动态和私有端口
           熟知端口范围为0-1023，不能个人使用。
           注册端口范围为1024-49151，必须向IANA注册,才能使用。
           动态和私有端口范围为49152-65535，无需注册就可以使用。
           可以从下面的地址，看到最新的端口分配情况。
           关于端口号，查看：
           http://www.iana.org/assignments/port-numbers
        */
        if (nroute)
            dst.sin_addr.s_addr = route[0];

        /*
           多route
        */
        if (connect (probe_fd, (struct sockaddr *) &dst, sizeof (dst)) == -1)
        {

            /*
               试探目的主机的基本情况：是不是广播地址、网络是不是工作正常等
            */
            if (errno == EACCES)
            {

                /*
                   在没有设置socket广播地址标志时候，使用connect尝试连接一个广播地址，
                   会出错，将errno设置为EACCES
                */
                if (broadcast_pings == 0)
                {

                    /*
                       没有设置允连接广播地址
                    */
                    fprintf (stderr, "Do you want to ping broadcast? Then -b\n");
                    exit (2);
                }
                fprintf (stderr, "WARNING: pinging broadcast address\n");
                if (setsockopt (probe_fd, SOL_SOCKET, SO_BROADCAST, &broadcast_pings, sizeof (broadcast_pings)) < 0)
                {

                    /*
                       设置连接广播地址标志
                       SO_BROADCAST:Set or get the broadcast flag.
                    */
                    perror ("can't set broadcasting");
                    exit (2);
                }
                if (connect (probe_fd, (struct sockaddr *) &dst, sizeof (dst)) == -1)
                {

                    /*
                       设置连接广播地址标志后再次连接
                    */ perror ("connect");
                    exit (2);
                }
            }
            else
            {

                /*
                   非由于连接广播地址而出现的错误
                */
                perror ("connect");
                exit (2);
            }
        }
        alen = sizeof (source);
        if (getsockname (probe_fd, (struct sockaddr *) &source, &alen) == -1)
        {

            /*
               赋给source当前socket的名字，为什么这样做？
               Unix网络编程提到：
               getsockname中的“name”实际上是一个误导，
               函数返回与某网络连接任一端口关联的协议地址，对于IP4和IPv6，它是IP地址和端口号的组合，与域名没有任何联系
               又谈到：
               在捆绑了一个通配IP地址的TCP服务器上，一旦与客户建立了连接（connet成功返回），就可以调用getsocket来获得分配给此连接的本地IP地址。
               可见，可能在一般环境中看起来没有用的语句，在特殊环境中却是必不可少的。
               总之，source现在确定无疑地获得了本地IP地址
            */
            perror ("getsockname");
            exit (2);
        }
        source.sin_port = 0;

        /*
           设置source的端口号为0
        */
        close (probe_fd);
        lixiprintf ("probe Ok\n");
    }
    while (0);
    if (whereto.sin_addr.s_addr == 0)
        whereto.sin_addr.s_addr = source.sin_addr.s_addr;

    if (icmp_sock < 0)
    {

        /*
           icmp_sock是在上面申请的一个socket
        */
        errno = socket_errno;
        perror ("ping: icmp open socket");
        exit (2);
    }

    if (device)
    {
        struct ifreq ifr;

        memset (&ifr, 0, sizeof (ifr));
        strncpy (ifr.ifr_name, device, IFNAMSIZ - 1);

        /*
           设备名设置为device
        */
        if (ioctl (icmp_sock, SIOCGIFINDEX, &ifr) < 0)
        {

            /*
               SIOCGIFINDEX：接口的索引
            */
            fprintf (stderr, "ping: unknown iface %s\n", device);
            exit (2);
        }
        cmsg.ipi.ipi_ifindex = ifr.ifr_ifindex;
        cmsg_len = sizeof (cmsg);
    }

    if (broadcast_pings || IN_MULTICAST (ntohl (whereto.sin_addr.s_addr)))
    {

        /*
           程序运行到这里，就表明-b参数被设置了
        */
        lixiprintf ("broadcast_pings is setted, but it does not neccessarily mean that the address is MULTICAST\n");
        if (uid)
        {
            if (interval < 1000)
            {
                fprintf (stderr, "ping: broadcast ping with too short interval.\n");
                exit (2);
            }
            if (pmtudisc >= 0 && pmtudisc != IP_PMTUDISC_DO)
            {

                /*
                   非超级用户不允许在ping广播地址时进行分段
                */
                fprintf (stderr, "ping: broadcast ping does not fragment.\n");
                exit (2);
            }
        }
        if (pmtudisc < 0)

            /*
               程序运行到这里表明pmtudisc没有被-M参数设置
               pmtudisc可以被设置为IP_PMTUDISC_DO、IP_PMTUDISC_DONT或者IP_PMTUDISC_WANT
            */
            pmtudisc = IP_PMTUDISC_DO;
    }

    if (pmtudisc >= 0)
    {

        /*
            设置了IP_PMTUDISC_*
            #define IP_PMTUDISC_DONT        0     //Never send DF frames
            #define IP_PMTUDISC_WANT        1     //Use per route hints
            #define IP_PMTUDISC_DO            2     //Always DF，即不允许分段
        */

        if (setsockopt (icmp_sock, SOL_IP, IP_MTU_DISCOVER, &pmtudisc, sizeof (pmtudisc)) == -1)
        {

            /*
               设置IP_MTU_DISCOVER
               linux会根据RFC 1191中的方法进行找出Path MTU，在ip的标志（3bit）位的第二位（Do Not Fragment）设置为1，即不分段
               IP_MTU_DISCOVER:Sets or receives the Path MTU Discovery setting for a socket.
            */
            perror ("ping: IP_MTU_DISCOVER");
            exit (2);
        }
    }

    if ((options & F_STRICTSOURCE) && bind (icmp_sock, (struct sockaddr *) &source, sizeof (source)) == -1)
    {

        /*
           -I <hoseName>
           给套接口分配一个协议地址，协议地址包括地址及端口
        */
        perror ("bind");
        exit (2);
    }

    if (1)
    {
        lixiprintf ("first icmp_filter installed\n");
        struct icmp_filter filt;

        /*
           icmp_filter类型的值对每种ICMP消息都有一个位（掩码），能把那种ICMP消息过滤掉。如果不设置则不过滤ICMP消息。
        */
        filt.data = ~((1 << ICMP_SOURCE_QUENCH) |

                /*
                   Source Quench
                */
                (1 << ICMP_DEST_UNREACH) |

                /*
                   Destination Unreachable
                */
                (1 << ICMP_TIME_EXCEEDED) |

                /*
                   Time Exceeded
                */
                (1 << ICMP_PARAMETERPROB) |

                /*Parameter Problem*/
                (1 << ICMP_REDIRECT) |

                /*
                   Echo Reply
                */
                (1 << ICMP_ECHOREPLY));

        /*
           Echo Reply
        */
        if (setsockopt (icmp_sock, SOL_RAW, ICMP_FILTER, (char *) &filt, sizeof (filt)) == -1)

            /*
               设置套接字ICMP过滤选项
               过滤ICMP消息
               ICMP_FILTER：Enable a special filter for raw sockets bound to the IPPROTO_ICMP protocol.
            */
            perror ("WARNING: setsockopt(ICMP_FILTER)");
    }

    hold = 1;
    if (setsockopt (icmp_sock, SOL_IP, IP_RECVERR, (char *) &hold, sizeof (hold)))
        fprintf (stderr, "WARNING: your kernel is veeery old. No problems.\n");

    /*
       IP_RECVERR：允许传递扩展的可靠的错误信息。
       如果在数据报上激活了该标识, 那么所有产生的错误会在每套接字一个的错误队列中排队等待。
       IP_MTU, IP_MTU_DISCOVER, IP_PKTINFO, IP_RECVERR and IP_ROUTER_ALERT are new options in Linux 2.2. They are also all Linux specific and should not be used in programs intended to be portable.
    */

    /* record route option */
    if (options & F_RROUTE)
    {
        memset (rspace, 0, sizeof (rspace));
        rspace[0] = IPOPT_NOP;

        /*
           IPOPT_NOP和IPOPT_NOOP等效 都表示没有操作
        */
        rspace[1 + IPOPT_OPTVAL] = IPOPT_RR;

        /*
           IPOPT_RR表示记录路由信息
        */
        rspace[1 + IPOPT_OLEN] = sizeof (rspace) - 1;

        /*
           IPOPT_OLENL：选项长度
        */
        rspace[1 + IPOPT_OFFSET] = IPOPT_MINOFF;

        /*
           IPOPT_OLENL：下一项的偏移量
        */
        optlen = 40;
        if (setsockopt (icmp_sock, IPPROTO_IP, IP_OPTIONS, rspace, sizeof (rspace)) < 0)
        {

            /*
               IP_OPTIONS:设置socket的IP选项
            */
            perror ("ping: record route");
            exit (2);
        }
    }
    if (options & F_TIMESTAMP)
    {
        memset (rspace, 0, sizeof (rspace));
        rspace[0] = IPOPT_TIMESTAMP;

        /*
           指明IP选项的类型是时间戳
        */
        rspace[1] = (ts_type == IPOPT_TS_TSONLY ? 40 : 36);

        /*
           时间绰选项的总长度为40或者36byte
           为什么是IPOPT_TS_TSONLY就设置为40否则设置为36？
        */
        rspace[2] = 5;

        /*
           指向下一个存放时间戳的地址，由于时间戳占4byte，故此该值按5，9，13变化
        */
        rspace[3] = ts_type;

        /*
           将标志字段设置为对应的时间戳选项
        */
        if (ts_type == IPOPT_TS_PRESPEC)
        {

            /*
               如果是-T tsprespec [host1 [host2 [host3 [host4]]]] 选项
               预先存入各主机地址
            */
            int i;

            rspace[1] = 4 + nroute * 8;
            for (i = 0; i < nroute; i++)
                *(__u32 *) & rspace[4 + i * 8] = route[i];

            /*
               注意和F_SOURCEROUTE被设置时计算对比
               这里递加8，而那里递加4；这里保留存储时间戳的位置，而那里连续存储
            */
        }
        if (setsockopt (icmp_sock, IPPROTO_IP, IP_OPTIONS, rspace, rspace[1]) < 0)
        {

            /*
               设置套接口设置，IP选项中会放入将各IP地址
               IP_OPTIONS:Sets or get the IP options to be sent with every packet from this socket.
            */
            rspace[3] = 2;

            /*
                #define    IPOPT_TS_TSONLY        0
                #define    IPOPT_TS_TSANDADDR    1
                #define    IPOPT_TS_PRESPEC    3
                设置为2，再尝试一遍
            */
            if (setsockopt (icmp_sock, IPPROTO_IP, IP_OPTIONS, rspace, rspace[1]) < 0)
            {
                perror ("ping: ts option");
                exit (2);
            }
        }
        optlen = 40;
    }
    if (options & F_SOURCEROUTE)
    {
        int i;

        memset (rspace, 0, sizeof (rspace));
        rspace[0] = IPOPT_NOOP;

        /*
           IPOPT_NOOP表示无操作。
        */
        rspace[1 + IPOPT_OPTVAL] = (options & F_SO_DONTROUTE) ? IPOPT_SSRR : IPOPT_LSRR;

        /*
           IPOPT_OPTVAL：选项类型
           IPOPT_LSRR：Loose Source Route
           IPOPT_SSRR：Strict Source Route
        */
        rspace[1 + IPOPT_OLEN] = 3 + nroute * 4;

        /*
           IPOPT_OLENL：选项长度
        */
        rspace[1 + IPOPT_OFFSET] = IPOPT_MINOFF;

        /*
           IPOPT_OLENL：下一个选项偏移量
           IPOPT_MINOFF：值为4
        */
        for (i = 0; i < nroute; i++)
            *(__u32 *) & rspace[4 + i * 4] = route[i];

        if (setsockopt (icmp_sock, IPPROTO_IP, IP_OPTIONS, rspace, 4 + nroute * 4) < 0)
        {

            /*
               设置套接口设置，IP选项中会放入将各IP地址
            */
            perror ("ping: record route");
            exit (2);
        }
        optlen = 40;
    }

    /* Estimate memory eaten by single packet. It is rough estimate.
     * Actually, for small datalen's it depends on kernel side a lot. */
    hold = datalen + 8;

    /*
       ICMP报文的大小
    */
    hold += ((hold + 511) / 512) * (optlen + 20 + 16 + 64 + 160);

    /*
       不知道这个是怎么估算的
       optlen：IP报文选项长度？
       20：IP报文固定长度？
       ...
    */
    lixiprintf ("Estimate memory :%d\n", hold);
    sock_setbufs (icmp_sock, hold);

    /*
       hold为估计的发送缓冲区的大小
    */
    if (broadcast_pings)
    {
        if (setsockopt (icmp_sock, SOL_SOCKET, SO_BROADCAST, &broadcast_pings, sizeof (broadcast_pings)) < 0)
        {

            /*
               设置socket为允许发送广播数据报
               SO_BROADCAS:Set or get the broadcast flag.
            */
            perror ("ping: can't set broadcasting");
            exit (2);
        }
    }

    if (options & F_NOLOOP)
    {
        int loop = 0;

        if (setsockopt (icmp_sock, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, 1) == -1)
        {

            /*
               设置socket为禁止多播数据包的回馈
               IP_MULTICAST_LOOP:Sets or reads a boolean integer argument whether sent multicast packets should be looped back to the local sockets.
            */
            perror ("ping: can't disable multicast loopback");
            exit (2);
        }
    }
    if (options & F_TTL)
    {
        int ittl = ttl;

        if (setsockopt (icmp_sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, 1) == -1)
        {

            /*
               设置输出组播数据的TTL值
               IP_MULTICAST_TTL:Set or reads the time-to-live value of outgoing multicast packets for this socket.
            */
            perror ("ping: can't set multicast time-to-live");
            exit (2);
        }
        if (setsockopt (icmp_sock, IPPROTO_IP, IP_TTL, &ittl, sizeof (ittl)) == -1)
        {

            /*
               设置socket指定TTL存活时间
               IP_TTL:Set or retrieve the current time to live field that is used in every packet sent from this socket.
            */
            perror ("ping: can't set unicast time-to-live");
            exit (2);
        }
    }

    if (datalen > 0xFFFF - 8 - optlen - 20)
    {

        /*
           ICMP的数据长度超过了最大限制
           这个最大长度是：IP数据最大长度减去ICMP的头长度8byte，再减去IP头的固定长度20，再减去IP选项的长度optlen
           IP数据最大长度受IP头中总长度字段为16bit的限制，为2^16-1=65535byte
        */
        if (uid || datalen > sizeof (outpack) - 8)
        {

            /*
               只有超级用户才能超过以上的限制，但是绝对不能超过一下限制
               ICMP数据长加上ICMP的头长度的限制：不能超过程序存储的空间
               不过我暂时还不明白，允许超级用户设置数据长度大于一个IP报文能发送的数据长度有何用途？
            */
            fprintf (stderr, "Error: packet size %d is too large. Maximum is %d\n", datalen, 0xFFFF - 8 - 20 - optlen);
            exit (2);
        }
        /* Allow small oversize to root yet. It will cause EMSGSIZE. */
        fprintf (stderr, "WARNING: packet size %d is too large. Maximum is %d\n", datalen, 0xFFFF - 8 - 20 - optlen);
    }

    if (datalen >= sizeof (struct timeval))    /* can we time transfer */
        timing = 1;
    packlen = datalen + MAXIPLEN + MAXICMPLEN;
    if (!(packet = (u_char *) malloc ((u_int) packlen)))
    {
        fprintf (stderr, "ping: out of memory.\n");
        exit (2);
    }

    printf ("PING %s (%s) ", hostname, inet_ntoa (whereto.sin_addr));
    if (device || (options & F_STRICTSOURCE))
        printf ("from %s %s: ", inet_ntoa (source.sin_addr), device ? : "");
    printf ("%d(%d) bytes of data.\n", datalen, datalen + 8 + optlen + 20);

    /*
       "datalen"是ICMP的数据部分，默认值为56
       "8"是ICMP的8字节首部
       datalen+8是ICMP报文的大小，即IP数据报的数据部分的大小
       "optlen"是IP的头选项的大小，最小为0，最大值为40
       "20"是IP头固定部分的大小
       optlen + 20是IP头部的大小
       datalen+8+optlen+20是IP数据报文的总大小
    */

    lixiprintf ("sizeof(outpack)-8:%d\n", sizeof (outpack) - 8);
    lixiprintf ("sizeof(struct timeval):%d\n", sizeof (struct timeval));
    lixiprintf ("LONG_MAX:%ld\n", LONG_MAX);
    lixiprintf ("MAX_DUP_CHK:%d\n", MAX_DUP_CHK);
    lixiprintf ("INT_MAX:%d\n", INT_MAX);
    lixiprintf ("npackets:%ld\n", npackets);
    lixiprintf ("sndbuf:%d\n", sndbuf);
    lixiprintf ("uid:%d\n", uid);
    setup (icmp_sock);

    main_loop (icmp_sock, packet, packlen);

}


int receive_error_msg ()
{
    int res;

    char cbuf[512];

    struct iovec iov;

    struct msghdr msg;

    struct cmsghdr *cmsg;

    struct sock_extended_err *e;

    struct icmphdr icmph;

    struct sockaddr_in target;

    int net_errors = 0;

    int local_errors = 0;

    int saved_errno = errno;

    /*
       注意:这里保存了errno的旧值，才调用有可能修改errno值的函数recvmsg()
    */
    iov.iov_base = &icmph;
    iov.iov_len = sizeof (icmph);
    msg.msg_name = (void *) &target;
    msg.msg_namelen = sizeof (target);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_flags = 0;
    msg.msg_control = cbuf;
    msg.msg_controllen = sizeof (cbuf);

    res = recvmsg (icmp_sock, &msg, MSG_ERRQUEUE | MSG_DONTWAIT);

    /*
       SOL_IP中设置了允许传递扩展的可靠的错误信息标识IP_RECVERR，所有产生的错误会在每套接字一个的错误队列中排队等待。当用户从套接字操作中收到错误时，就可以通过调用设置了 MSG_ERRQUEUE 标识的 recvmsg()来接收。
       MSG_DONTWAIT：设置为不可阻断运作
    */
    if (res < 0)
        goto out;

    e = NULL;
    for (cmsg = CMSG_FIRSTHDR (&msg); cmsg; cmsg = CMSG_NXTHDR (&msg, cmsg))
    {
        if (cmsg->cmsg_level == SOL_IP)
        {
            if (cmsg->cmsg_type == IP_RECVERR)

                /*
                   描述错误的sock_extended_err结构将通过一条类型为IP_RECVERR, 级别为 SOL_IP的辅助信息进行传递
                */
                e = (struct sock_extended_err *) CMSG_DATA (cmsg);

            /*
               没有break;语句
               一直找到最后一个满足条件的附属数据对象，而不是第一个满足条件的附属数据对象
            */
        }
    }
    if (e == NULL)
        abort ();

    if (e->ee_origin == SO_EE_ORIGIN_LOCAL)
    {

        /*
           IP 按照下面的方法使用 sock_extended_err 结构：
           1. 对于本地产生的错误ee_origin设为SO_EE_ORIGIN_LOCAL
           2. ICMP包接收的错误ee_origin设为SO_EE_ORIGIN_ICMP
           本地出错
        */
        local_errors++;
        if (options & F_QUIET)
            goto out;
        if (options & F_FLOOD)
            write (STDOUT_FILENO, "E", 1);
        else if (e->ee_errno != EMSGSIZE)
            fprintf (stderr, "ping: local error: %s\n", strerror (e->ee_errno));
        else
            fprintf (stderr, "ping: local error: Message too long, mtu=%u\n", e->ee_info);
        nerrors++;

        /*
           为什么要放在最后，不放在和local_errors++;一起?
           如果为静默模式的话，就会把这句跳过(对比下面的类似的代码）。
        */
    }
    else if (e->ee_origin == SO_EE_ORIGIN_ICMP)
    {

        /*
           ICMP差错报文
        */
        struct sockaddr_in *sin = (struct sockaddr_in *) (e + 1);

        if (res < sizeof (icmph) ||
                target.sin_addr.s_addr != whereto.sin_addr.s_addr || icmph.type != ICMP_ECHO || icmph.un.echo.id != ident)
        {
            /* Not our error, not an error at all. Clear. */
            saved_errno = 0;
            goto out;
        }

        /*
           ICMP差错报文的源主机地址和本机主机地址、ICMP类型和ICMP_ECHO、报文中的标识符和本进程ID都符合
        */
        acknowledge (ntohs (icmph.un.echo.sequence));
        if (!working_recverr)
        {
            struct icmp_filter filt;

            working_recverr = 1;
            /* OK, it works. Add stronger filter. */
            filt.data = ~((1 << ICMP_SOURCE_QUENCH) | (1 << ICMP_REDIRECT) | (1 << ICMP_ECHOREPLY));
            if (setsockopt (icmp_sock, SOL_RAW, ICMP_FILTER, (char *) &filt, sizeof (filt)) == -1)

                /*
                   如果是网络出错则安装一个更加严格的过滤器
                   ICMP_FILTER:Enable a special filter for raw sockets bound to the IPPROTO_ICMP protocol.
                */
                perror ("\rWARNING: setsockopt(ICMP_FILTER)");
        }

        net_errors++;
        nerrors++;
        if (options & F_QUIET)
            goto out;
        if (options & F_FLOOD)
        {
            write (STDOUT_FILENO, "\bE", 2);
        }
        else
        {
            printf ("From %s icmp_seq=%u ", pr_addr (sin->sin_addr.s_addr), ntohs (icmph.un.echo.sequence));

            /*
               ICMP差错报文源主机地址，序列号
            */
            pr_icmph (e->ee_type, e->ee_code, e->ee_info, NULL);

            /*
               分析并打印出网络出错原因
            */
            fflush (stdout);
        }
    }

out:
    errno = saved_errno;
    return net_errors ? : -local_errors;

    /*
       返回正数表明是网络错误，返回负数表明是本地错误；错误个数是返回值的绝对值。
    */
}

/*
 * pinger --
 *     Compose and transmit an ICMP ECHO REQUEST packet.  The IP packet
 * will be added on by the kernel.  The ID field is our UNIX process ID,
 * and the sequence number is an ascending integer.  The first 8 bytes
 * of the data portion are used to hold a UNIX "timeval" struct in VAX
 * byte-order, to compute the round-trip time.
 */
int send_probe ()
{
    struct icmphdr *icp;

    int cc;

    int i;

    icp = (struct icmphdr *) outpack;
    icp->type = ICMP_ECHO;
    icp->code = 0;
    icp->checksum = 0;
    icp->un.echo.sequence = htons (ntransmitted + 1);
    icp->un.echo.id = ident;    /* ID */

    CLR ((ntransmitted + 1) % mx_dup_ck);

    if (timing)
    {

        /*
           如果ICMP的数据大小不够就不做timing
        */
        if (options & F_LATENCY)
        {

            /*

*/
            static volatile int fake_fucked_egcs = sizeof (struct timeval);

            struct timeval tmp_tv;

            gettimeofday (&tmp_tv, NULL);
            /* egcs is crap or glibc is crap, but memcpy
               does not copy anything, if len is constant! */

            /*
               作者居然骂F...
               hiahia
            */
            memcpy (icp + 1, &tmp_tv, fake_fucked_egcs);
        }
        else
        {
            memset (icp + 1, 0, sizeof (struct timeval));
        }
    }

    cc = datalen + 8;            /* skips ICMP portion */

    /* compute ICMP checksum here */
    icp->checksum = in_cksum ((u_short *) icp, cc, 0);

    if (timing && !(options & F_LATENCY))
    {

        /*
           在ICMP的数据中放入时间数据
        */
        static volatile int fake_fucked_egcs = sizeof (struct timeval);

        struct timeval tmp_tv;

        gettimeofday (&tmp_tv, NULL);
        /* egcs is crap or glibc is crap, but memcpy
           does not copy anything, if len is constant! */
        memcpy (icp + 1, &tmp_tv, fake_fucked_egcs);

        /*
           ICMP报文是8byte头和可变长的数据部分组成
           icp+1就指向了数据部分
           数据部分存储了发送时刻的时间
        */
        lixiprintf ("<tmp_tv> copyed to:%dth byte of <icp>\n", (int) (icp + 1) - (int) (icp));
        icp->checksum = in_cksum ((u_short *) (icp + 1), fake_fucked_egcs, ~icp->checksum);
    }

    do
    {
        static struct iovec iov = { outpack, 0 };
        static struct msghdr m = { &whereto, sizeof (whereto),
            &iov, 1, &cmsg, 0, 0
        };
        m.msg_controllen = cmsg_len;
        iov.iov_len = cc;

        i = sendmsg (icmp_sock, &m, confirm);

        /*
           发送ICMP消息
           On success, these calls return the number of characters sent. On error, -1 is returned, and errno is set appropriately.
        */
        confirm = 0;
    }
    while (0);

    return (cc == i ? 0 : i);

    /*
       返回-1，表示出错；
       返回0，表示正确；
    */
}

/*
 * parse_reply --
 *    Print out the packet, if it came from us.  This logic is necessary
 * because ALL readers of the ICMP socket get a copy of ALL ICMP packets
 * which arrive ('tis only fair).  This permits multiple copies of this
 * program to be run without having intermingled output (or statistics!).
 */
int parse_reply (struct msghdr *msg, int cc, void *addr, struct timeval *tv)
{
    struct sockaddr_in *from = addr;

    __u8 *buf = msg->msg_iov->iov_base;

    struct icmphdr *icp;

    struct iphdr *ip;

    int hlen;

    int csfailed;

    /* Check the IP header */
    ip = (struct iphdr *) buf;
    hlen = ip->ihl * 4;
    if (cc < hlen + 8 || ip->ihl < 5)
    {
        if (options & F_VERBOSE)
            fprintf (stderr, "ping: packet too short (%d bytes) from %s\n", cc, pr_addr (from->sin_addr.s_addr));
        return 1;
    }

    /* Now the ICMP part */
    cc -= hlen;
    icp = (struct icmphdr *) (buf + hlen);
    csfailed = in_cksum ((u_short *) icp, cc, 0);

    if (icp->type == ICMP_ECHOREPLY)
    {

        /*
           是ICMP的回复
        */
        if (icp->un.echo.id != ident)

            /*
               ICMP的回复的ID不是本进程的ID
            */
            return 1;            /* 'Twas not our ECHO */

        if (gather_statistics ((__u8 *) (icp + 1), cc,
                    ntohs (icp->un.echo.sequence), ip->ttl, 0, tv, pr_addr (from->sin_addr.s_addr)))

            /*本进程的ICMP的回复*/
            return 0;
    }
    else
    {

        /*
           如果不是ICMP的回复
        */
        /* We fall here when a redirect or source quench arrived.
         * Also this branch processes icmp errors, when IP_RECVERR
         * is broken. */

        switch (icp->type)
        {
            case ICMP_ECHO:
                /* MUST NOT */
                return 1;
            case ICMP_SOURCE_QUENCH:
            case ICMP_REDIRECT:
            case ICMP_DEST_UNREACH:
            case ICMP_TIME_EXCEEDED:
            case ICMP_PARAMETERPROB:
                {
                    struct iphdr *iph = (struct iphdr *) (&icp[1]);

                    struct icmphdr *icp1 = (struct icmphdr *) ((unsigned char *) iph + iph->ihl * 4);

                    int error_pkt;

                    if (cc < 8 + sizeof (struct iphdr) + 8 || cc < 8 + iph->ihl * 4 + 8)
                        return 1;
                    if (icp1->type != ICMP_ECHO || iph->daddr != whereto.sin_addr.s_addr || icp1->un.echo.id != ident)
                        return 1;
                    error_pkt = (icp->type != ICMP_REDIRECT && icp->type != ICMP_SOURCE_QUENCH);
                    if (error_pkt)
                    {
                        acknowledge (ntohs (icp1->un.echo.sequence));
                        if (working_recverr)
                        {
                            return 0;
                        }
                        else
                        {
                            static int once;

                            /* Sigh, IP_RECVERR for raw socket
                             * was broken until 2.4.9. So, we ignore
                             * the first error and warn on the second.
                          */
                            if (once++ == 1)
                                fprintf (stderr, "\rWARNING: kernel is not very fresh, upgrade is recommended.\n");
                            if (once == 1)
                                return 0;
                        }
                    }
                    nerrors += error_pkt;
                    if (options & F_QUIET)
                        return !error_pkt;
                    if (options & F_FLOOD)
                    {
                        if (error_pkt)
                            write (STDOUT_FILENO, "\bE", 2);
                        return !error_pkt;
                    }
                    printf ("From %s: icmp_seq=%u ", pr_addr (from->sin_addr.s_addr), ntohs (icp1->un.echo.sequence));
                    if (csfailed)
                        printf ("(BAD CHECKSUM)");
                    pr_icmph (icp->type, icp->code, ntohl (icp->un.gateway), icp);
                    return !error_pkt;
                }
            default:
                /* MUST NOT */
                break;
        }
        if ((options & F_FLOOD) && !(options & (F_VERBOSE | F_QUIET)))
        {
            if (!csfailed)
                write (STDOUT_FILENO, "!E", 2);
            else
                write (STDOUT_FILENO, "!EC", 3);
            return 0;
        }
        if (!(options & F_VERBOSE) || uid)
            return 0;
        printf ("From %s: ", pr_addr (from->sin_addr.s_addr));
        if (csfailed)
        {
            printf ("(BAD CHECKSUM)\n");
            return 0;
        }
        pr_icmph (icp->type, icp->code, ntohl (icp->un.gateway), icp);
        return 0;
    }

    if (!(options & F_FLOOD))
    {
        pr_options (buf + sizeof (struct iphdr), hlen);

        if (options & F_AUDIBLE)
            putchar ('\a');
        putchar ('\n');
        fflush (stdout);
    }
    return 0;
}

u_short in_cksum (const u_short * addr, register int len, u_short csum)
{

    /*
       让我想起了教材上的一道习题
       证明校验和可以通过如下方式计算：
       首先对32比特单元中的缓冲区求32bit的反码和，然后对高半字和低半字求16bit反码和，最后同样对结果求反
       （为了在32bi的补码硬件上求32bit的反码和，需要访问“溢出”位）
       不过这里没有使用上面的算法，而是用32bit的加法器把所有16bit的数加起来（可能最后还有1byte的落单），
       然后将32bit的高16bit加上低16bit（注意还要叠合一次，因为反码加法的溢出都要加到最地位），再取反就可以了
    */
    register int nleft = len;

    const u_short *w = addr;

    register u_short answer;

    register int sum = csum;

    /*
     *  Our algorithm is simple, using a 32 bit accumulator (sum),
     *  we add sequential 16 bit words to it, and at the end, fold
     *  back all the carry bits from the top 16 bits into the lower
     *  16 bits.
  */
    while (nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }

    /* mop up an odd byte, if necessary */
    if (nleft == 1)
        sum += htons (*(u_char *) w << 8);

    /*
     * add back carry outs from top 16 bits to low 16 bits
  */
    sum = (sum >> 16) + (sum & 0xffff);    /* add hi 16 to low 16 */

    /*
       高16bit是原来所有16bit（当然，落单的8bit加法也可能产生16bit进位）加法的进位次数
    */
    sum += (sum >> 16);            /* add carry */

    /*
       注意还要叠合一次，因为反码加法的溢出都要加到最低位
    */
    answer = ~sum;                /* truncate to 16 bits */
    return (answer);
}

/*
 * pr_icmph --
 *    Print a descriptive string about an ICMP header.
 */
void pr_icmph (__u8 type, __u8 code, __u32 info, struct icmphdr *icp)
{

    /*
       分析出错原因
       参看ICMP报文类型
    */
    switch (type)
    {
        case ICMP_ECHOREPLY:
            printf ("Echo Reply\n");
            /* XXX ID + Seq + Data */
            break;
        case ICMP_DEST_UNREACH:
            switch (code)
            {
                case ICMP_NET_UNREACH:
                    printf ("Destination Net Unreachable\n");
                    break;
                case ICMP_HOST_UNREACH:
                    printf ("Destination Host Unreachable\n");
                    break;
                case ICMP_PROT_UNREACH:
                    printf ("Destination Protocol Unreachable\n");
                    break;
                case ICMP_PORT_UNREACH:
                    printf ("Destination Port Unreachable\n");
                    break;
                case ICMP_FRAG_NEEDED:
                    printf ("Frag needed and DF set (mtu = %u)\n", info);
                    break;
                case ICMP_SR_FAILED:
                    printf ("Source Route Failed\n");
                    break;
                case ICMP_PKT_FILTERED:
                    printf ("Packet filtered\n");
                    break;
                default:
                    printf ("Dest Unreachable, Bad Code: %d\n", code);
                    break;
            }
            if (icp && (options & F_VERBOSE))
                pr_iph ((struct iphdr *) (icp + 1));
            break;
        case ICMP_SOURCE_QUENCH:
            printf ("Source Quench\n");
            if (icp && (options & F_VERBOSE))
                pr_iph ((struct iphdr *) (icp + 1));
            break;
        case ICMP_REDIRECT:
            switch (code)
            {
                case ICMP_REDIR_NET:
                    printf ("Redirect Network");
                    break;
                case ICMP_REDIR_HOST:
                    printf ("Redirect Host");
                    break;
                case ICMP_REDIR_NETTOS:
                    printf ("Redirect Type of Service and Network");
                    break;
                case ICMP_REDIR_HOSTTOS:
                    printf ("Redirect Type of Service and Host");
                    break;
                default:
                    printf ("Redirect, Bad Code: %d", code);
                    break;
            }
            if (icp)
                printf ("(New nexthop: %s)\n", pr_addr (icp->un.gateway));
            if (icp && (options & F_VERBOSE))
                pr_iph ((struct iphdr *) (icp + 1));
            break;
        case ICMP_ECHO:
            printf ("Echo Request\n");
            /* XXX ID + Seq + Data */
            break;
        case ICMP_TIME_EXCEEDED:
            switch (code)
            {
                case ICMP_EXC_TTL:
                    printf ("Time to live exceeded\n");
                    break;
                case ICMP_EXC_FRAGTIME:
                    printf ("Frag reassembly time exceeded\n");
                    break;
                default:
                    printf ("Time exceeded, Bad Code: %d\n", code);
                    break;
            }
            if (icp && (options & F_VERBOSE))
                pr_iph ((struct iphdr *) (icp + 1));
            break;
        case ICMP_PARAMETERPROB:
            printf ("Parameter problem: pointer = %u\n", icp ? (ntohl (icp->un.gateway) >> 24) : info);
            if (icp && (options & F_VERBOSE))
                pr_iph ((struct iphdr *) (icp + 1));
            break;
        case ICMP_TIMESTAMP:
            printf ("Timestamp\n");
            /* XXX ID + Seq + 3 timestamps */
            break;
        case ICMP_TIMESTAMPREPLY:
            printf ("Timestamp Reply\n");
            /* XXX ID + Seq + 3 timestamps */
            break;
        case ICMP_INFO_REQUEST:
            printf ("Information Request\n");
            /* XXX ID + Seq */
            break;
        case ICMP_INFO_REPLY:
            printf ("Information Reply\n");
            /* XXX ID + Seq */
            break;
#ifdef ICMP_MASKREQ
        case ICMP_MASKREQ:
            printf ("Address Mask Request\n");
            break;
#endif
#ifdef ICMP_MASKREPLY
        case ICMP_MASKREPLY:
            printf ("Address Mask Reply\n");
            break;
#endif
        default:
            printf ("Bad ICMP type: %d\n", type);
    }
}


void pr_options (unsigned char *cp, int hlen)
{
    int i, j;

    int optlen, totlen;

    unsigned char *optptr;

    static int old_rrlen;

    static char old_rr[MAX_IPOPTLEN];

    totlen = hlen - sizeof (struct iphdr);
    optptr = cp;

    while (totlen > 0)
    {
        if (*optptr == IPOPT_EOL)
            break;
        if (*optptr == IPOPT_NOP)
        {
            totlen--;
            optptr++;
            printf ("\nNOP");
            continue;
        }
        cp = optptr;
        optlen = optptr[1];
        if (optlen < 2 || optlen > totlen)
            break;

        switch (*cp)
        {
            case IPOPT_SSRR:
            case IPOPT_LSRR:
                printf ("\n%cSRR: ", *cp == IPOPT_SSRR ? 'S' : 'L');
                j = *++cp;
                i = *++cp;
                i -= 4;
                cp++;
                if (j > IPOPT_MINOFF)
                {
                    for (;;)
                    {
                        __u32 address;

                        memcpy (&address, cp, 4);
                        cp += 4;
                        if (address == 0)
                            printf ("\t0.0.0.0");
                        else
                            printf ("\t%s", pr_addr (address));
                        j -= 4;
                        putchar ('\n');
                        if (j <= IPOPT_MINOFF)
                            break;
                    }
                }
                break;
            case IPOPT_RR:
                j = *++cp;        /* get length */
                i = *++cp;        /* and pointer */
                if (i > j)
                    i = j;
                i -= IPOPT_MINOFF;
                if (i <= 0)
                    continue;
                if (i == old_rrlen && !strncmp ((char *) cp, old_rr, i) && !(options & F_FLOOD))
                {
                    printf ("\t(same route)");
                    i = ((i + 3) / 4) * 4;
                    cp += i;
                    break;
                }
                old_rrlen = i;
                memcpy (old_rr, (char *) cp, i);
                printf ("\nRR: ");
                cp++;
                for (;;)
                {
                    __u32 address;

                    memcpy (&address, cp, 4);
                    cp += 4;
                    if (address == 0)
                        printf ("\t0.0.0.0");
                    else
                        printf ("\t%s", pr_addr (address));
                    i -= 4;
                    putchar ('\n');
                    if (i <= 0)
                        break;
                }
                break;
            case IPOPT_TS:
                {
                    int stdtime = 0, nonstdtime = 0;

                    __u8 flags;

                    j = *++cp;    /* get length */
                    i = *++cp;    /* and pointer */
                    if (i > j)
                        i = j;
                    i -= 5;
                    if (i <= 0)
                        continue;
                    flags = *++cp;
                    printf ("\nTS: ");
                    cp++;
                    for (;;)
                    {
                        long l;

                        if ((flags & 0xF) != IPOPT_TS_TSONLY)
                        {
                            __u32 address;

                            memcpy (&address, cp, 4);
                            cp += 4;
                            if (address == 0)
                                printf ("\t0.0.0.0");
                            else
                                printf ("\t%s", pr_addr (address));
                            i -= 4;
                            if (i <= 0)
                                break;
                        }
                        l = *cp++;
                        l = (l << 8) + *cp++;
                        l = (l << 8) + *cp++;
                        l = (l << 8) + *cp++;

                        if (l & 0x80000000)
                        {
                            if (nonstdtime == 0)
                                printf ("\t%ld absolute not-standard", l & 0x7fffffff);
                            else
                                printf ("\t%ld not-standard", (l & 0x7fffffff) - nonstdtime);
                            nonstdtime = l & 0x7fffffff;
                        }
                        else
                        {
                            if (stdtime == 0)
                                printf ("\t%ld absolute", l);
                            else
                                printf ("\t%ld", l - stdtime);
                            stdtime = l;
                        }
                        i -= 4;
                        putchar ('\n');
                        if (i <= 0)
                            break;
                    }
                    if (flags >> 4)
                        printf ("Unrecorded hops: %d\n", flags >> 4);
                    break;
                }
            default:
                printf ("\nunknown option %x", *cp);
                break;
        }
        totlen -= optlen;
        optptr += optlen;
    }
}

/*
 * pr_iph --
 *    Print an IP header with options.
 */
void pr_iph (struct iphdr *ip)
{
    int hlen;

    u_char *cp;

    hlen = ip->ihl << 2;
    cp = (u_char *) ip + 20;    /* point to options */

    printf ("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst Data\n");
    printf (" %1x  %1x  %02x %04x %04x", ip->version, ip->ihl, ip->tos, ip->tot_len, ip->id);
    printf ("   %1x %04x", ((ip->frag_off) & 0xe000) >> 13, (ip->frag_off) & 0x1fff);
    printf ("  %02x  %02x %04x", ip->ttl, ip->protocol, ip->check);
    printf (" %s ", inet_ntoa (*(struct in_addr *) &ip->saddr));
    printf (" %s ", inet_ntoa (*(struct in_addr *) &ip->daddr));
    printf ("\n");
    pr_options (cp, hlen);
}

/*
 * pr_addr --
 *    Return an ascii host address as a dotted quad and optionally with
 * a hostname.
 */
char *pr_addr (__u32 addr)
{
    struct hostent *hp;

    static char buf[4096];

    if ((options & F_NUMERIC) || !(hp = gethostbyaddr ((char *) &addr, 4, AF_INET)))

        /*
           只要设置了F_NUMERIC就不用调用gethostbyaddr来查询DNS主机来核对了
        */
        sprintf (buf, "%s", inet_ntoa (*(struct in_addr *) &addr));
    else
        snprintf (buf, sizeof (buf), "%s (%s)", hp->h_name, inet_ntoa (*(struct in_addr *) &addr));
    return (buf);
}


/* Set Type of Service (TOS) and other Quality of Service relating bits */
int parsetos (char *str)
{
    const char *cp;

    int tos;

    char *ep;

    /* handle both hex and decimal values */
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
    {
        cp = str + 2;
        tos = (int) strtol (cp, &ep, 16);
    }
    else
        tos = (int) strtol (str, &ep, 10);

    /* doesn't look like decimal or hex, eh? */
    if (*ep != '\0')
    {
        fprintf (stderr, "ping: \"%s\" bad value for TOS\n", str);
        exit (2);
    }

    if (tos > TOS_MAX)
    {
        fprintf (stderr, "ping: the decimal value of TOS bits must be 0-254 (or zero)\n");
        exit (2);
    }
    return (tos);
}

#include <linux/filter.h>

void install_filter (void)
{
    lixiprintf ("filter installed\n");
    static int once;

    static struct sock_filter insns[] = {
        BPF_STMT (BPF_LDX | BPF_B | BPF_MSH, 0),    /* Skip IP header. F..g BSD... Look into ping6. */

        /*
           BPF_LDX+BPF_B+BPF_MSH
           X <- 4*(P[k:1]&0xf)
        */
        BPF_STMT (BPF_LD | BPF_H | BPF_IND, 4),    /* Load icmp echo ident */

        /*
           BPF_LD+BPF_H+BPF_IND
           A <- P[X+k:2]
        */
        BPF_JUMP (BPF_JMP | BPF_JEQ | BPF_K, 0xAAAA, 0, 1),    /* Ours? */

        /*
           BPF_JMP+BPF_JEQ+BPF_K
           pc += (A == k) ? jt : jf
        */
        BPF_STMT (BPF_RET | BPF_K, ~0U),    /* Yes, it passes. */

        /*
           BPF_RET+BPF_K
           accept k bytes
        */
        BPF_STMT (BPF_LD | BPF_B | BPF_IND, 0),    /* Load icmp type */

        /*
           BPF_LD+BPF_B+BPF_IND
           A <- P[X+k:1]
        */
        BPF_JUMP (BPF_JMP | BPF_JEQ | BPF_K, ICMP_ECHOREPLY, 1, 0),    /* Echo? */

        /*
           BPF_JMP+BPF_JEQ+BPF_K
           pc += (A == k) ? jt : jf
        */
        BPF_STMT (BPF_RET | BPF_K, 0xFFFFFFF),    /* No. It passes. */

        /*
           BPF_RET+BPF_K
           accept k bytes
        */
        BPF_STMT (BPF_RET | BPF_K, 0)    /* Echo with wrong ident. Reject. */

            /*
               BPF_RET+BPF_K
               accept k bytes
            */
    };

    /*
       BPF：BSD分组过滤器
    */
    static struct sock_fprog filter = {
        sizeof insns / sizeof (insns[0]),
        insns
    };

    if (once)

        /*
           如果设置过了就不再设置了
        */
        return;
    once = 1;

    /* Patch bpflet for current identifier. */
    insns[2] = (struct sock_filter) BPF_JUMP (BPF_JMP | BPF_JEQ | BPF_K, htons (ident), 0, 1);

    /*
       好吧，将那个不伦不类的0xAAAA改成ident对应的标识符
    */
    if (setsockopt (icmp_sock, SOL_SOCKET, SO_ATTACH_FILTER, &filter, sizeof (filter)))

        /*
           filter
           安装过滤器
        */
        perror ("WARNING: failed to install socket filter\n");
}


void usage (void)
{

    /*
       报错函数
    */
    fprintf (stderr,
            "lixi: ping [-LRUbdfnqrvVaA] [-c count] [-i interval] [-w deadline]\n"
            "            [-p pattern] [-s packetsize] [-t ttl] [-I interface or address]\n"
            "            [-M mtu discovery hint] [-S sndbuf]\n"
            "            [ -T timestamp option ] [ -Q tos ] [hop1 ...] destination\n");
    exit (2);
}
