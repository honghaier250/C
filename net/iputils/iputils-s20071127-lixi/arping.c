
/*
 * arping.c
 *
 *        This program is free software; you can redistribute it and/or
 *        modify it under the terms of the GNU General Public License
 *        as published by the Free Software Foundation; either version
 *        2 of the License, or (at your option) any later version.
 *
 * Authors:    Alexey Kuznetsov, <kuznet@ms2.inr.ac.ru>
 */

#include <stdlib.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <linux/sockios.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <net/if_arp.h>
#include <sys/uio.h>

#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "SNAPSHOT.h"

static void usage (void) __attribute__ ((noreturn));

int quit_on_reply = 0;

char *device = "eth0";

int ifindex;

char *source;

struct in_addr src, dst;

char *target;

int dad, unsolicited, advert;

int quiet;

int count = -1;

int timeout;

int unicasting;

int s;

int broadcast_only;

struct sockaddr_ll me;

struct sockaddr_ll he;

/*
struct sockaddr_ll
{
    unsigned short    sll_family;
    __be16        sll_protocol;
    int        sll_ifindex;
    unsigned short    sll_hatype;
    unsigned char    sll_pkttype;
    unsigned char    sll_halen;
    unsigned char    sll_addr[8];
};
linux-2.6.27/include/linux/if_packet.h
*/

struct timeval start, last;

int sent, brd_sent;

int received, brd_recv, req_recv;

#define MS_TDIFF(tv1,tv2) ( ((tv1).tv_sec-(tv2).tv_sec)*1000 + \
               ((tv1).tv_usec-(tv2).tv_usec)/1000 )

#define lixiprintf printf
void usage (void)
{
    fprintf (stderr,
             "Usage: arping [-fqbDUAV] [-c count] [-w timeout] [-I device] [-s source] destination\n"
             "  -f : quit on first reply\n"
             "  -q : be quiet\n"
             "  -b : keep broadcasting, don't go unicast\n"
             "  -D : duplicate address detection mode\n"
             "  -U : Unsolicited ARP mode, update your neighbours\n"
             "  -A : ARP answer mode, update your neighbours\n"
             "  -V : print version and exit\n"
             "  -c count : how many packets to send\n"
             "  -w timeout : how long to wait for a reply\n"
             "  -I device : which ethernet device to use (eth0)\n"
             "  -s source : source ip address\n" "  destination : ask for what ip address\n");
    exit (2);
}

void set_signal (int signo, void (*handler) (void))
{
    struct sigaction sa;

    memset (&sa, 0, sizeof (sa));
    sa.sa_handler = (void (*)(int)) handler;
    sa.sa_flags = SA_RESTART;
    sigaction (signo, &sa, NULL);
}

int send_pack (int s, struct in_addr src, struct in_addr dst, struct sockaddr_ll *ME, struct sockaddr_ll *HE)
{
    int err;

    struct timeval now;

    unsigned char buf[256];

    struct arphdr *ah = (struct arphdr *) buf;

    unsigned char *p = (unsigned char *) (ah + 1);

/*
struct arphdr
{
    __be16        ar_hrd;         //format of hardware address    
    __be16        ar_pro;         //format of protocol address    
    unsigned char    ar_hln;         //length of hardware address    
    unsigned char    ar_pln;         //length of protocol address    
    __be16        ar_op;         //ARP opcode (command)        
};
参见ARP请求或者应答分组格式
*/
    ah->ar_hrd = htons (ME->sll_hatype);
    if (ah->ar_hrd == htons (ARPHRD_FDDI))
        ah->ar_hrd = htons (ARPHRD_ETHER);
    ah->ar_pro = htons (ETH_P_IP);
    lixiprintf ("sll_hatype:%d:\n", ME->sll_hatype);

/*
硬件地址的类型，1表示以太网地址
*/
    ah->ar_hln = ME->sll_halen;
    lixiprintf ("sll_halen:%d:\n", ME->sll_halen);
    ah->ar_pln = 4;

/*
硬件地址长度和协议地址长度，以字节为单位。对于以太网上IP地址的ARP请求和应答来说，他们的值分别为6和4
*/
    ah->ar_op = advert ? htons (ARPOP_REPLY) : htons (ARPOP_REQUEST);

/*
操作（op）字段，值为1表示ARP请求，值为2表示ARP应答
如果是免费ARP模式，则设置为ARP请求(ARPOP_REPLY)，否则设置为ARP应答(ARPOP_REQUEST)
*/

    memcpy (p, &ME->sll_addr, ah->ar_hln);
    p += ME->sll_halen;

/*
发送端以太网地址
*/

    memcpy (p, &src, 4);
    p += 4;

/*
发送端IP地址
*/

    if (advert)

/*
免费ARP模式将目的以太网地址设置为本机以太网地址
而一般的模式则将目的以太网地址设置为目的主机以太网地址
*/
        memcpy (p, &ME->sll_addr, ah->ar_hln);
    else
        memcpy (p, &HE->sll_addr, ah->ar_hln);
    p += ah->ar_hln;

    memcpy (p, &dst, 4);
    p += 4;

/*
目的IP地址
*/
    gettimeofday (&now, NULL);
    err = sendto (s, buf, p - buf, 0, (struct sockaddr *) HE, sizeof (*HE));

/*
发送
*/
    if (err == p - buf)
    {
        last = now;
        sent++;
        if (!unicasting)
            brd_sent++;

/*
广播的报文次数
*/
    }
    return err;
}

void finish (void)
{
    if (!quiet)
    {
        printf ("Sent %d probes (%d broadcast(s))\n", sent, brd_sent);
        printf ("Received %d response(s)", received);
        if (brd_recv || req_recv)
        {
            printf (" (");
            if (req_recv)
                printf ("%d request(s)", req_recv);
            if (brd_recv)
                printf ("%s%d broadcast(s)", req_recv ? ", " : "", brd_recv);
            printf (")");
        }
        printf ("\n");
        fflush (stdout);
    }
    if (dad)
        exit (!!received);

/*
所有的退出程序，返回值都是2，1或者0
返回2表示程序发送ARP报文不成功。其他表示至少报文发出去了。
返回1表示ARP的回复出现意外情况，也就是本来重复地址检测希望不接受到ARP回复，但是却接受到了，而其他的情况下却没有接受到ARP回复
返回0表示没有出错
这里一个特殊情况就是，免费ARP（gratuitous ARP）不管有没有接到ARP回复都认为很正常。
*/
    if (unsolicited)
        exit (0);
    exit (!received);

}

void catcher (void)
{
    struct timeval tv;

    gettimeofday (&tv, NULL);

    if (start.tv_sec == 0)
        start = tv;
    lixiprintf ("count:%d\n", count);
    if (count-- == 0 || (timeout && MS_TDIFF (tv, start) > timeout * 1000 + 500))
        finish ();

/*
退出条件：
1. 要求发送的报文个数发送完毕
2. 设置时间限制的情况下，超过时间限制
逻辑上，在前面加一个if(count != -1)就完满了
否则会回绕到0，然后退出，不过要回绕也不是那么容易，因为回绕要经历2^32秒，也就是1362年，海枯石烂啊～
*/

    if (last.tv_sec == 0 || MS_TDIFF (tv, last) > 500)
    {

/*
从没有发送过，或者上次发送到现在的时间超过500毫秒
*/
        send_pack (s, src, dst, &me, &he);

/*
发送ARP报文
*/
        if (count == 0 && unsolicited)
            finish ();
    }
    alarm (1);

/*
过1秒后产生一个SIGALRM中断，从而再次进入cather函数
*/

/*
可以使用如下程序，加快中断产生速度，测试证明：在进入recv_pack()之前，计时器中断被屏蔽。
    struct itimerval it;
    it.it_interval.tv_sec = 0;
    it.it_interval.tv_usec = 0;
    it.it_value.tv_sec = 1/1000000;
    it.it_value.tv_usec = 10000%1000000;
    setitimer(ITIMER_REAL, &it, NULL);
*/

}

void print_hex (unsigned char *p, int len)
{
    int i;

    for (i = 0; i < len; i++)
    {
        printf ("%02X", p[i]);
        if (i != len - 1)
            printf (":");
    }
}

int recv_pack (unsigned char *buf, int len, struct sockaddr_ll *FROM)
{
    struct timeval tv;

    struct arphdr *ah = (struct arphdr *) buf;

    unsigned char *p = (unsigned char *) (ah + 1);

    struct in_addr src_ip, dst_ip;

    gettimeofday (&tv, NULL);

    /* Filter out wild packets */
    if (FROM->sll_pkttype != PACKET_HOST &&
        FROM->sll_pkttype != PACKET_BROADCAST && FROM->sll_pkttype != PACKET_MULTICAST)
        return 0;

/*
单播、广播或者多播
*/

    /* Only these types are recognised */
    if (ah->ar_op != htons (ARPOP_REQUEST) && ah->ar_op != htons (ARPOP_REPLY))
        return 0;

/*
操作类型（op）：
ARP请求或者ARP应答
*/

    /* ARPHRD check and this darned FDDI hack here :-( */
    if (ah->ar_hrd != htons (FROM->sll_hatype) &&
        (FROM->sll_hatype != ARPHRD_FDDI || ah->ar_hrd != htons (ARPHRD_ETHER)))
        return 0;

    /* Protocol must be IP. */
    if (ah->ar_pro != htons (ETH_P_IP))
        return 0;

/*
硬件地址的类型，为以太网地址类型
*/
    if (ah->ar_pln != 4)
        return 0;

/*
协议地址长度，为4
*/
    if (ah->ar_hln != me.sll_halen)
        return 0;

/*
硬件地址长度，为本机硬件地址长度
*/
    if (len < sizeof (*ah) + 2 * (4 + ah->ar_hln))
        return 0;
    memcpy (&src_ip, p + ah->ar_hln, 4);

/*
报文中记录的发送端IP地址
*/
    memcpy (&dst_ip, p + ah->ar_hln + 4 + ah->ar_hln, 4);

/*
报文中记录的目的IP地址
*/
    if (!dad)
    {
        if (src_ip.s_addr != dst.s_addr)
            return 0;

/*
收到的报文的发送端IP地址，为程序要探测的目的主机IP地址
*/
        if (src.s_addr != dst_ip.s_addr)
            return 0;

/*
收到的报文的目的IP地址，为本地主机IP地址
*/
        if (memcmp (p + ah->ar_hln + 4, &me.sll_addr, ah->ar_hln))

/*
报文中记录的目的硬件地址，为本地硬件地址
*/
            return 0;
    }
    else
    {
        /* DAD packet was:
           src_ip = 0 (or some src)
           src_hw = ME
           dst_ip = tested address
           dst_hw = <unspec>

           We fail, if receive request/reply with:
           src_ip = tested_address
           src_hw != ME
           if src_ip in request was not zero, check
           also that it matches to dst_ip, otherwise
           dst_ip/dst_hw do not matter.
         */
        if (src_ip.s_addr != dst.s_addr)

/*
收到的报文的发送端IP地址，为程序要探测的目的主机IP地址
*/
            return 0;
        if (memcmp (p, &me.sll_addr, me.sll_halen) == 0)

/*
报文中记录的发送端硬件地址，为本地主机硬件地址
*/
            return 0;
        if (src.s_addr && src.s_addr != dst_ip.s_addr)

/*
在DAD模式下，如果发送的ARP请求报文的源主机地址不是0.0.0.0，则判断：
收到的报文的目的IP地址，是不是源主机地址。如果不是则返回0。
注意：DAD模式下也可以通过-s选项设置一个允许的源主机地址，这样收到的ARP应答报文的目的IP地址就应该和ARP请求报文的源IP地址相符。
如果源主机没有设置（为0.0.0.0），则不用判断两者是否相等了。
*/
            return 0;
    }
    if (!quiet)
    {

/*不是静默模式就打印消息*/
        int s_printed = 0;

        printf ("%s ", FROM->sll_pkttype == PACKET_HOST ? "Unicast" : "Broadcast");
        printf ("%s from ", ah->ar_op == htons (ARPOP_REPLY) ? "reply" : "request");
        printf ("%s [", inet_ntoa (src_ip));
        print_hex (p, ah->ar_hln);
        printf ("] ");
        if (dst_ip.s_addr != src.s_addr)
        {
            printf ("for %s ", inet_ntoa (dst_ip));
            s_printed = 1;
        }
        if (memcmp (p + ah->ar_hln + 4, me.sll_addr, ah->ar_hln))
        {
            if (!s_printed)
                printf ("for ");
            printf ("[");
            print_hex (p + ah->ar_hln + 4, ah->ar_hln);
            printf ("]");
        }
        if (last.tv_sec)
        {
            long usecs = (tv.tv_sec - last.tv_sec) * 1000000 + tv.tv_usec - last.tv_usec;

            long msecs = (usecs + 500) / 1000;

            usecs -= msecs * 1000 - 500;
            printf (" %ld.%03ldms\n", msecs, usecs);
        }
        else
        {
            printf (" UNSOLICITED?\n");
        }
        fflush (stdout);
    }
    received++;
    if (FROM->sll_pkttype != PACKET_HOST)
        brd_recv++;
    if (ah->ar_op == htons (ARPOP_REQUEST))
        req_recv++;
    if (quit_on_reply)
        finish ();
    if (!broadcast_only)
    {

/*
如果不是只能对所有MAC地址地进行广播
在接到一个ARP回复之后就知道了目的主机的MAC地址
就可以设置目的主机的MAC地址，而不是维持ff:ff:ff:ff:ff:ff，即不再广播，而改成单播
*/
        memcpy (he.sll_addr, p, me.sll_halen);
        unicasting = 1;

/*
以后就进行单播了
*/
    }
    return 1;
}

int main (int argc, char **argv)
{
    int socket_errno;

    int ch;

    uid_t uid = getuid ();

    s = socket (PF_PACKET, SOCK_DGRAM, 0);

/*
PF_PACKET中的PF是 protocol family(协议族)的缩写。 
分组套接字被用于在设备层(OSI 的链路层) 收发原始(raw)分组。它允许用户在用户空间实现在物理层之上的协议模块。 
linux-2.6.27/include/linux/socket.h：
#define PF_PACKET    AF_PACKET
*/
    socket_errno = errno;

    if (setuid (uid))
    {
        perror ("arping: setuid");
        exit (-1);
    }

    while ((ch = getopt (argc, argv, "h?bfDUAqc:w:s:I:V")) != EOF)
    {
        switch (ch)
        {
            case 'b':
                broadcast_only = 1;
                break;
            case 'D':
                dad++;

/*
重复地址检测模式
*/
                quit_on_reply = 1;
                break;
            case 'U':
                unsolicited++;
                break;
            case 'A':
                advert++;
                unsolicited++;
                break;
            case 'q':
                quiet++;
                break;
            case 'c':
                count = atoi (optarg);
                break;
            case 'w':
                timeout = atoi (optarg);
                break;
            case 'I':
                device = optarg;
                break;
            case 'f':
                quit_on_reply = 1;
                break;
            case 's':
                source = optarg;
                break;
            case 'V':
                printf ("arping utility, iputils-ss%s\n", SNAPSHOT);
                exit (0);
            case 'h':
            case '?':
            default:
                usage ();
        }
    }
    argc -= optind;
    argv += optind;

    if (argc != 1)
        usage ();

    target = *argv;

    if (device == NULL)
    {
        fprintf (stderr, "arping: device (option -I) is required\n");
        usage ();
    }

    if (s < 0)
    {
        errno = socket_errno;
        perror ("arping: socket");
        exit (2);
    }

    if (1)
    {
        struct ifreq ifr;

        memset (&ifr, 0, sizeof (ifr));
        strncpy (ifr.ifr_name, device, IFNAMSIZ - 1);
        if (ioctl (s, SIOCGIFINDEX, &ifr) < 0)
        {

/*
SIOCGIFINDEX：Retrieve the interface index of the interface into ifr_ifindex. 
*/
            fprintf (stderr, "arping: unknown iface %s\n", device);
            exit (2);
        }
        ifindex = ifr.ifr_ifindex;
        lixiprintf ("ifindex:%d\n", ifindex);
        if (ioctl (s, SIOCGIFFLAGS, (char *) &ifr))
        {

/*
SIOCGIFFLAGS:Get or set the active flag word of the device.
*/
            perror ("ioctl(SIOCGIFFLAGS)");
            exit (2);
        }
        if (!(ifr.ifr_flags & IFF_UP))
        {

/*
IFF_UP:Interface is running
*/
            if (!quiet)
                printf ("Interface \"%s\" is down\n", device);
            exit (2);
        }
        if (ifr.ifr_flags & (IFF_NOARP | IFF_LOOPBACK))
        {

/*
IFF_NOARP:No arp protocol, L2 destination address not set. 
IFF_LOOPBACK:Interface is a loopback interface.
*/
            if (!quiet)
                printf ("Interface \"%s\" is not ARPable\n", device);
            exit (dad ? 0 : 2);
        }
    }

    if (inet_aton (target, &dst) != 1)
    {

/*
inet_aton将target中以数字和点的形式的因特网地址转换为二进制数据并存储在struct in_addr结构中
*/
        struct hostent *hp;

        hp = gethostbyname2 (target, AF_INET);

/*
如果target里面保存的地址不是以数字和点的形式则通过gethostbyname2()函数查询DNS获得地址
gethostbyname2()和gethostbyname()类似，但是与之不同的是多了一个参数，这里是AF_INET，表明地址是IPv4类型。
*/
        if (!hp)
        {
            fprintf (stderr, "arping: unknown host %s\n", target);
            exit (2);
        }
        memcpy (&dst, hp->h_addr, 4);
    }

    if (source && inet_aton (source, &src) != 1)
    {

/*
源IP地址被用户设置好了
*/
        fprintf (stderr, "arping: invalid source %s\n", source);
        exit (2);
    }

    if (!dad && unsolicited && src.s_addr == 0)
        src = dst;

    if (!dad || src.s_addr)
    {

/*
非重复地址检测模式，或者源地址已经设置
如果是DAD模式，则src.s_addr一直没有设置，那么就意味着源地址为0.0.0.0。因为进行D重复地址检测模式的原因很可能是由于源主机的IP地址没有设置，从而想设置自身的IP地址。
*/
        struct sockaddr_in saddr;

        int probe_fd = socket (AF_INET, SOCK_DGRAM, 0);

/*
UDP的套接字
*/
        if (probe_fd < 0)
        {
            perror ("socket");
            exit (2);
        }
        if (device)
        {
            if (setsockopt (probe_fd, SOL_SOCKET, SO_BINDTODEVICE, device, strlen (device) + 1) == -1)

/*
这个操作似乎只有超级用户才能设置成功
SO_BINDTODEVICE:Bind this socket to a particular device like "eth0", as specified in the passed interface name. 
*/
                perror ("WARNING: interface is ignored");
        }
        memset (&saddr, 0, sizeof (saddr));
        saddr.sin_family = AF_INET;
        if (src.s_addr)
        {

/*
源地址已经设置
*/
            saddr.sin_addr = src;
            if (bind (probe_fd, (struct sockaddr *) &saddr, sizeof (saddr)) == -1)
            {
                lixiprintf ("1st bind error\n");
                perror ("bind");
                exit (2);
            }
        }
        else if (!dad)
        {

/*
如果是DAD，不执行这些操作取得源IP地址
*/
            int on = 1;

            socklen_t alen = sizeof (saddr);

            saddr.sin_port = htons (1025);
            saddr.sin_addr = dst;

            if (setsockopt (probe_fd, SOL_SOCKET, SO_DONTROUTE, (char *) &on, sizeof (on)) == -1)

/*
SO_DONTROUTE:Don't send via a gateway
*/
                perror ("WARNING: setsockopt(SO_DONTROUTE)");
            if (connect (probe_fd, (struct sockaddr *) &saddr, sizeof (saddr)) == -1)
            {
                perror ("connect");
                exit (2);
            }
            if (getsockname (probe_fd, (struct sockaddr *) &saddr, &alen) == -1)
            {

/*
建立连接之后，就可以调用getsocket来获得分配给此连接的本地IP地址。
与ping.c中作用相仿
*/
                perror ("getsockname");
                exit (2);
            }
            src = saddr.sin_addr;

/*
查路由表，取得了本地IP地址
*/
        }
        close (probe_fd);
    };

    me.sll_family = AF_PACKET;

/*
总是AF_PACKET类型
*/
    me.sll_ifindex = ifindex;

/*
Interface number 
*/
    me.sll_protocol = htons (ETH_P_ARP);

/*
协议类型：以太网ARP
ETH_P_ARP：Address Resolution packet
/include/linux/if_ether.h
*/
    if (bind (s, (struct sockaddr *) &me, sizeof (me)) == -1)
    {

/*
bind函数给套接字分配一个本地协议地址
*/
        lixiprintf ("2nd bind error\n");
        perror ("bind");
        exit (2);
    }

    if (1)
    {
        socklen_t alen = sizeof (me);

        if (getsockname (s, (struct sockaddr *) &me, &alen) == -1)
        {
            perror ("getsockname");
            exit (2);
        }
    }
    if (me.sll_halen == 0)
    {
        if (!quiet)
            printf ("Interface \"%s\" is not ARPable (no ll address)\n", device);
        exit (dad ? 0 : 2);
    }

    he = me;
    memset (he.sll_addr, -1, he.sll_halen);

/*
把目的主机地址设为ff:ff:ff:ff:ff:ff  即广播地址,当然假设是以太网
*/

    if (!quiet)
    {
        printf ("ARPING %s ", inet_ntoa (dst));
        printf ("from %s %s\n", inet_ntoa (src), device ? : "");
    }

    if (!src.s_addr && !dad)
    {

/*
在非DAD模式下，没有取得IP地址
*/
        fprintf (stderr, "arping: no source address in not-DAD mode\n");
        exit (2);
    }
    lixiprintf ("get IP:%s\n", inet_ntoa (src));
    set_signal (SIGINT, finish);
    set_signal (SIGALRM, catcher);

/*
安装中断处理程序
*/
    catcher ();

/*
没过1秒就会进入这个函数，发送一个ARP报文
*/

    while (1)
    {
        sigset_t sset, osset;

        unsigned char packet[4096];

        struct sockaddr_ll from;

        socklen_t alen = sizeof (from);

        int cc;

        if ((cc = recvfrom (s, packet, sizeof (packet), 0, (struct sockaddr *) &from, &alen)) < 0)
        {
            perror ("arping: recvfrom");
            continue;
        }

/*
接受报文
*/
        sigemptyset (&sset);

/*
sigemptyset() initializes the signal set given by set to empty, with all signals excluded from the set. 
*/
        sigaddset (&sset, SIGALRM);
        sigaddset (&sset, SIGINT);

/*
将SIGALRM、SIGINT添加到信号量集中
*/
        sigprocmask (SIG_BLOCK, &sset, &osset);

/*
最后一个参数不为NULL，所以将原来的信号掩码保存起来以后再用。
将sset中的掩码和原来的信号掩码合并起来作为新的信号掩码，这里的作用就是为了recv_pack()函数不被中断，新屏蔽了SIGALRM中断和SIGINT中断。
函数解释：
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset); 
如果oldset不是NULL指针的话，当前的信号掩码会保存到这里
如果set不是NULL指针的话，第一个参数how决定了怎么处理set所包含的信号掩码的处理方式
how共有三个值:
SIG_BLOCK 设置当前的信号掩码(signal mask)为set和原有信号掩码的并集
SIG_UNBLOCK set是我们想从原有信号掩码中unblock掉的信号
SIG_SETMASK 用set来替换原有的信号掩码
*/
        lixiprintf ("Before recv_pack()\n");
        recv_pack (packet, cc, &from);
        lixiprintf ("After recv_pack()\n");

/*
处理报文
*/
        sigprocmask (SIG_SETMASK, &osset, NULL);

/*
最后一个参数为NULL，当前的信号掩码不被保存。
将上面保存的信号掩码（即没有添加SIGALRM、SIGINT信号掩码之前的信号掩码）重新设定。
*/
    }
}
