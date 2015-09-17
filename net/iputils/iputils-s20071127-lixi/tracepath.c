
/*
 * tracepath.c
 *
 *        This program is free software; you can redistribute it and/or
 *        modify it under the terms of the GNU General Public License
 *        as published by the Free Software Foundation; either version
 *        2 of the License, or (at your option) any later version.
 *
 * Authors:    Alexey Kuznetsov, <kuznet@ms2.inr.ac.ru>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/errqueue.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <arpa/inet.h>

#ifndef IP_PMTUDISC_PROBE
#define IP_PMTUDISC_PROBE    3
#endif

#define lixiprintf printf
int lixicount;

struct hhistory
{
    int hops;
    struct timeval sendtime;
};

struct hhistory his[64];

int hisptr;

struct sockaddr_in target;

__u16 base_port;

const int overhead = 28;

int mtu = 65535;

int hops_to = -1;

int hops_from = -1;

int no_resolve = 0;

struct probehdr
{
    __u32 ttl;
    struct timeval tv;
};

void data_wait (int fd)
{
    fd_set fds;

    struct timeval tv;

    FD_ZERO (&fds);
    FD_SET (fd, &fds);
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    select (fd + 1, &fds, NULL, NULL, &tv);
}

int recverr (int fd, int ttl)
{
    int res;

    struct probehdr rcvbuf;

    char cbuf[512];

    struct iovec iov;

    struct msghdr msg;

    struct cmsghdr *cmsg;

    struct sock_extended_err *e;

    struct sockaddr_in addr;

    struct timeval tv;

    struct timeval *rettv;

    int slot;

    int rethops;

    int sndhops;

    int progress = -1;

    int broken_router;

  restart:
    memset (&rcvbuf, -1, sizeof (rcvbuf));
    iov.iov_base = &rcvbuf;
    iov.iov_len = sizeof (rcvbuf);
    msg.msg_name = (__u8 *) & addr;
    msg.msg_namelen = sizeof (addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_flags = 0;
    msg.msg_control = cbuf;
    msg.msg_controllen = sizeof (cbuf);

    gettimeofday (&tv, NULL);
    res = recvmsg (fd, &msg, MSG_ERRQUEUE);

/*
SOL_IP中设置了允许传递扩展的可靠的错误信息标识IP_RECVERR，所有产生的错误会在每套接字一个的错误队列中排队等待。当用户从套接字操作中收到错误时，就可以通过调用设置了 MSG_ERRQUEUE 标识的 recvmsg()来接收。
MSG_ERRQUEUE：This flag specifies that queued errors should be received from the socket error queue. 
从错误队列中接受消息
recvmsg接受到数据时，返回接受到的信息总长度
如果没有信息可供接受同时表现出有秩序地关闭，则返回0
否则返回-1，设置errno为对应错误
*/
    lixicount++;
    lixiprintf ("lixicount:%d\n", lixicount);
    if (res < 0)
    {

/*
发生了错误
*/
        if (errno == EAGAIN)
        {
            lixiprintf ("progress Returned, progress:%d\n", progress);
            return progress;
        }

/*
EAGAIN：The socket's file descriptor is marked O_NONBLOCK and no data is waiting to be received;or MSG_OOB is set and no out-of-band data is available and either the socket's file descriptor is marked O_NONBLOCK or the socket does not support blocking to await out-of-band data. 
返回progress的值，progress初始为-1，经过一次循环之后即变为MTU

只有在这里recverr函数才能返回非0值
*/
        goto restart;
    }

    progress = mtu;

    rethops = -1;
    sndhops = -1;
    e = NULL;
    rettv = NULL;
    slot = ntohs (addr.sin_port) - base_port;

/*
先将端口地址的网络字节序转换为本地字节序
端口地址减去 base_port得到了his[slot]中的slot，his[slot]就存储了原UDP报文的ttl设置值和发送时间。
这里ICMP错误报文是对原UDP报文的回复
*/
    if (slot >= 0 && slot < 63 && his[slot].hops)
    {
        sndhops = his[slot].hops;
        rettv = &his[slot].sendtime;
        his[slot].hops = 0;
    }
    broken_router = 0;
    if (res == sizeof (rcvbuf))
    {
        if (rcvbuf.ttl == 0 || rcvbuf.tv.tv_sec == 0)
        {
            broken_router = 1;
        }
        else
        {
            sndhops = rcvbuf.ttl;
            rettv = &rcvbuf.tv;
        }
    }

    for (cmsg = CMSG_FIRSTHDR (&msg); cmsg; cmsg = CMSG_NXTHDR (&msg, cmsg))
    {

/*
struct cmsghdr *CMSG_FIRSTHDR(struct msghdr *msgh)
struct cmsghdr *CMSG_NXTHDR(struct msghdr *msgh, struct cmsghdr *cmsg);
CMSG_FIRSTHDR() returns a pointer to the first cmsghdr in the ancillary data buffer associated with the passed msghdr.
CMSG_NXTHDR() returns the next valid cmsghdr after the passed cmsghdr. It returns NULL when there isn't enough space left in the buffer. 
struct cmsghdr {
    socklen_t    cmsg_len;    //data byte count, including header 
    int    cmsg_level;    // originating protocol 
    int    cmsg_type;    // protocol-specific type 
// followed by  unsigned char    cmsg_data[]; 
};
struct msghdr {
    void         *msg_name;       // optional address 
    socklen_t     msg_namelen;    // size of address 
    struct iovec *msg_iov;        // scatter/gather array 
    size_t        msg_iovlen;     // # elements in msg_iov 
    void         *msg_control;    // ancillary data, see below 
    socklen_t     msg_controllen; // ancillary data buffer len 
    int           msg_flags;      // flags on received message 
};
*/
        if (cmsg->cmsg_level == SOL_IP)
        {
            if (cmsg->cmsg_type == IP_RECVERR)
            {
                e = (struct sock_extended_err *) CMSG_DATA (cmsg);

/*
取得消息
在辅助缓冲区查找IP_RECVERR选项
unsigned char *CMSG_DATA(struct cmsghdr *cmsg);
CMSG_DATA() returns a pointer to the data portion of a cmsghdr. 
*/
            }
            else if (cmsg->cmsg_type == IP_TTL)
            {

/*
在辅助缓冲区查找IP_TTL选项
*/
                rethops = *(int *) CMSG_DATA (cmsg);
            }
            else
            {
                printf ("cmsg:%d\n ", cmsg->cmsg_type);
            }
        }
    }
    if (e == NULL)
    {
        printf ("no info\n");
        return 0;

/*
没任何消息
*/
    }
    if (e->ee_origin == SO_EE_ORIGIN_LOCAL)
    {

/*
本地错误
SO_EE_ORIGIN_LOCAL：locally generated errors
*/
        printf ("%2d?: %-15s ", ttl, "[LOCALHOST]");
    }
    else if (e->ee_origin == SO_EE_ORIGIN_ICMP)
    {

/*
接受到ICMP差错报文
errors received as an ICMP packet,
*/
        char abuf[128];

        struct sockaddr_in *sin = (struct sockaddr_in *) (e + 1);

        inet_ntop (AF_INET, &sin->sin_addr, abuf, sizeof (abuf));

        if (sndhops > 0)
            printf ("%2d:  ", sndhops);
        else
            printf ("%2d?: ", ttl);

        if (!no_resolve)
        {
            char fabuf[256];

            struct hostent *h;

            fflush (stdout);
            h = gethostbyaddr ((char *) &sin->sin_addr, sizeof (sin->sin_addr), AF_INET);
            snprintf (fabuf, sizeof (fabuf), "%s (%s)", h ? h->h_name : abuf, abuf);
            printf ("%-52s ", fabuf);
        }
        else
        {
            printf ("%-15s ", abuf);
        }
    }

    if (rettv)
    {
        int diff = (tv.tv_sec - rettv->tv_sec) * 1000000 + (tv.tv_usec - rettv->tv_usec);

        printf ("%3d.%03dms ", diff / 1000, diff % 1000);

/*
rettv是发送的时间
tv是接受的时间
在这里打印出时间收发的时间间隔
*/
        //lixiprintf("yeah");
        if (broken_router)
            printf ("(This broken router returned corrupted payload) ");
    }

    switch (e->ee_errno)
    {

/*
处理错误
众多的错误定义在：
#include <errno.h>
*/
        case ETIMEDOUT:

/*
ETIMEDOUT:Connection timed out (POSIX.1) 
*/
            printf ("\n");
            break;
        case EMSGSIZE:

/*
MTU大小小于包的大小
修改MTU的值，确定以后的传输正确
EMSGSIZE：Packet too big. 
*/
            printf ("pmtu %d\n", e->ee_info);
            mtu = e->ee_info;
            progress = mtu;
            break;
        case ECONNREFUSED:

/*
到达了目的地主机
ECONNREFUSED:A remote host refused to allow the network connection (typically because it is not running the requested service). 
*/
            printf ("reached\n");
            hops_to = sndhops < 0 ? ttl : sndhops;
            hops_from = rethops;
            return 0;
        case EPROTO:

/*
EPROTO:An ICMP error has arrived reporting a parameter problem. 
*/
            printf ("!P\n");
            return 0;
        case EHOSTUNREACH:

/*
参看TCP/IP详解，卷1：协议，72页
ICMP超时报文有两类：代码为0或1
代码为0，表示TTL的值等于0的时候产生的
代码为1，表示主机在组装分片的时候发生超时
EHOSTUNREACH:Host is unreachable (POSIX.1) 
*/
            if (e->ee_origin == SO_EE_ORIGIN_ICMP && e->ee_type == 11 &&

/*
类型为11，表明是“超时”报文
代码为0，表示TTL的值等于0的时候产生的
*/
                e->ee_code == 0)
            {

/*
#define SO_EE_ORIGIN_NONE    0
#define SO_EE_ORIGIN_LOCAL   1
#define SO_EE_ORIGIN_ICMP    2
#define SO_EE_ORIGIN_ICMP6   3

struct sock_extended_err {
    u_int32_t ee_errno;   // error number 
    u_int8_t  ee_origin;  // where the error originated 
    u_int8_t  ee_type;    // type 
    u_int8_t  ee_code;    // code 
    u_int8_t  ee_pad;
    u_int32_t ee_info;    // additional information 
    u_int32_t ee_data;    // other data 
    // More data may follow 
};
SO_EE_ORIGIN_ICMP：errors received as an ICMP packet
在ICMP的的头部有类型和代码两个字段
ee_type就是类型字段的值
ee_code就是代码字段的值
*/
                if (rethops >= 0)
                {

/*
报文中携带了TTL的数据，rethops改变
rethops是还剩下多少生存时间
所以应该用最大生存时间减去rethops，得到用掉的生存时间
IP首部有一个8位的TTL字段，故此最大值为255
但是究竟会将这个TTL的初始值设为多少并不确定，
为什么要用65 129来减呢？....
traceroute好像也是这样实现的，
#define ttl2hops(X)    (((X) <= 64 ? 65 : ((X) <= 128 ? 129 : 256)) - (X))
正如tracepath的手册所说，这个信息是不可靠的
*/
                    if (rethops <= 64)
                    {
                        rethops = 65 - rethops;
                        lixiprintf (" Max TTL:64\n");
                    }
                    else if (rethops <= 128)
                    {
                        rethops = 129 - rethops;
                        lixiprintf (" Max TTL:128\n");
                    }
                    else
                    {
                        rethops = 256 - rethops;
                        lixiprintf (" Max TTL:255\n");
                    }
                    if (sndhops >= 0 && rethops != sndhops)

/*
接受到的ICMP错误报文中存储了原来IP报文中的TTL，但是sndhops却和rethops不一致
*/
                        printf ("asymm %2d ", rethops);
                    else if (sndhops < 0 && rethops != ttl)

/*
没有得到ICMP错误报文中存储的原来IP报文中的TTL
正是由于上面所说rethops的计算值的不可靠性,可能出现的问题是发送和收回报文计算出来的跳数不一致
*/
                        printf ("asymm %2d ", rethops);
                }
                printf ("\n");
                break;
            }
            printf ("!H\n");
            return 0;
        case ENETUNREACH:

/*
ENETUNREACH:Network is unreachable. 
*/
            printf ("!N\n");
            return 0;
        case EACCES:

/*
EACCES:
*/
            printf ("!A\n");
            return 0;
        default:
            printf ("\n");
            errno = e->ee_errno;
            perror ("NET ERROR");
            return 0;
    }
    goto restart;
}

int probe_ttl (int fd, int ttl)
{
    int i;

    char sndbuf[mtu];

    struct probehdr *hdr = (struct probehdr *) sndbuf;

    memset (sndbuf, 0, mtu);

  restart:
    for (i = 0; i < 10; i++)
    {

/*
尝试发送十次
如果十次都不成功就退出程序
*/
        int res;

        hdr->ttl = ttl;
        target.sin_port = htons (base_port + hisptr);

/*
将主机的字节序转换为网络的字节序，主机字节序各有不同，网络字节顺序采用big endian排序方式。
The htons() function converts the unsigned short integer hostshort from host byte order to network byte order. 
base_port：44444端口，或者用户设置的端口号
/////////////////////////
*/
        gettimeofday (&hdr->tv, NULL);
        his[hisptr].hops = ttl;
        his[hisptr].sendtime = hdr->tv;

/*
在his数组中存放发送时间和TTL
*/
        if (sendto (fd, sndbuf, mtu - overhead, 0, (struct sockaddr *) &target, sizeof (target)) > 0)
        {

/*
IP首部为20
UDP首部为8
故此应MTU减去28字节
发送报文成功，退出循环
*/
            lixiprintf ("sentto sin_port:0x%x, base_port:0x%x, hisptr:0x%x, \n", target.sin_port, base_port, hisptr);
            break;
        }
        lixiprintf ("fail to sentto\n");
        res = recverr (fd, ttl);

/*
接受并分析报文
*/
        his[hisptr].hops = 0;
        if (res == 0)
        {

/*
成功找到主机了，或者有不可恢复的错误
*/
            lixiprintf ("res = 0\n");
            return 0;
        }

/*
完成或者出现错误消息
*/
        if (res > 0)
        {

/*
没有发送且返回值是当前MTU，说明recverr()接受到错误信息是MTU太大。
mut变量已将在recverr()中修改了，所以只需要重新来
*/
            lixiprintf ("res > 0\n");
            goto restart;
        }

/*
如果res<0，则重新来一次，注意与res > 0情况重新来的区别
res<0表明一直发生EAGAIN错误，没有接受到一个ICMP错误报文
*/
    }
    hisptr = (hisptr + 1) & 63;

/*
每发送一个则断口号＋1
存储历史也＋1
64个回绕一次
*/
    if (i < 10)
    {

/*
十次以内将报文发送出去，并接受到想要的错误信息
*/
        data_wait (fd);
        if (recv (fd, sndbuf, sizeof (sndbuf), MSG_DONTWAIT) > 0)
        {

/*
在什么情况下出现？
接受到了UDP的回复，例如报文传送到了在运行对应端口的UDP服务主机
MSG_DONTWAIT：本操作不阻塞
接受到了信息，本来不应该接受到UDP回复的，所以打了个?号
*/
            printf ("%2d?: reply received 8)\n", ttl);
            return 0;
        }
        return recverr (fd, ttl);
    }

/*
十次都没有成功地发送
*/
    printf ("%2d:  send failed\n", ttl);
    return 0;
}

static void usage (void) __attribute ((noreturn));

     static void usage (void)
{
    fprintf (stderr, "Usage: tracepath [-n] [-l <len>] <destination>[/<port>]\n");
    exit (-1);
}

int main (int argc, char **argv)
{
    struct hostent *he;

    int fd;

    int on;

    int ttl;

    char *p;

    int ch;

    while ((ch = getopt (argc, argv, "nh?l:")) != EOF)
    {
        switch (ch)
        {
            case 'n':
                no_resolve = 1;
                break;
            case 'l':
                if ((mtu = atoi (optarg)) <= overhead)
                {
                    fprintf (stderr, "Error: length must be >= %d\n", overhead);
                    exit (1);
                }
                break;
            default:
                usage ();
        }
    }

    argc -= optind;
    argv += optind;

    if (argc != 1)
        usage ();

/*
除了一个ip地址和-n,-l参数之外没有其他
*/

    fd = socket (AF_INET, SOCK_DGRAM, 0);

/*
取得socket，建立一个UDP类型的socket
PF_INET：IPv4 Internet protocols
SOCK_DGRAM：Supports datagrams (connectionless, unreliable messages of a fixed maximum length). 
*/
    if (fd < 0)
    {
        perror ("socket");
        exit (1);
    }
    target.sin_family = AF_INET;

    p = strchr (argv[0], '/');

/*
取得'/'在argv中的地址
*/
    if (p)
    {

/*
如果地址中有端口，就使用这个端口，否则用44444端口
*/
        *p = 0;
        lixiprintf ("你设置了端口号\n");
        base_port = atoi (p + 1);
    }
    else
        base_port = 44444;
    he = gethostbyname (argv[0]);
    if (he == NULL)
    {
        herror ("gethostbyname");
        exit (1);
    }
    memcpy (&target.sin_addr, he->h_addr, 4);

/*
得到目的主机的地址
*/

    on = IP_PMTUDISC_PROBE;
    if (setsockopt (fd, SOL_IP, IP_MTU_DISCOVER, &on, sizeof (on)) &&
        (on = IP_PMTUDISC_DO, setsockopt (fd, SOL_IP, IP_MTU_DISCOVER, &on, sizeof (on))))
    {

/*
IP_MTU_DISCOVER：Sets or receives the Path MTU Discovery setting for a socket.
*/
        perror ("IP_MTU_DISCOVER");
        exit (1);
    }
    on = 1;
    if (setsockopt (fd, SOL_IP, IP_RECVERR, &on, sizeof (on)))
    {

/*
IP_RECVERR：Enable extended reliable error message passing. 
*/
        perror ("IP_RECVERR");
        exit (1);
    }
    if (setsockopt (fd, SOL_IP, IP_RECVTTL, &on, sizeof (on)))
    {

/*
IP_RECVTTL：When this flag is set pass a IP_TTL control message with the time to live field of the received packet as a byte. 
*/
        perror ("IP_RECVTTL");
        exit (1);
    }

    for (ttl = 1; ttl < 32; ttl++)
    {

/*
TTL从1到31，故此可以找到的最远的主机为31跳之外
*/
        int res;

        int i;

        on = ttl;
        if (setsockopt (fd, SOL_IP, IP_TTL, &on, sizeof (on)))
        {

/*
设置TTL
IP_TTL：Set or retrieve the current time to live field that is used in every packet sent from this socket. 
*/
            perror ("IP_TTL");
            exit (1);
        }

      restart:
        for (i = 0; i < 3; i++)
        {

/*
尝试三次
*/
            int old_mtu;

            old_mtu = mtu;
            res = probe_ttl (fd, ttl);
            if (mtu != old_mtu)
            {

/*
MTU被修改，重新试探这一跳的路由
如果没有设置MTU为一个合适的大小，而是让它为默认的65535，第一跳至少出现三次，即标号为1的输出至少出现三次
这两次中第一次就是试探出MTU
第一次是probe_ttl根本没有发送出去
第二次是probe_ttl改变MTU发了一次
第三次是这里看见mtu变量被改变，所以再次调用probe_ttl，重新尝试了一下。
如果很好的设置MTU为合适的大小，这三次就会变成只有一次了。
----------------------------------------------------------------------------------------
lixi@lixi-desktop:~$ tracepath 210.45.74.25
 1:  lixi-desktop.local (210.45.74.25)                      0.120ms pmtu 16436
 1:  lixi-desktop.local (210.45.74.25)                      0.052ms reached
 1:  lixi-desktop.local (210.45.74.25)                      0.043ms reached
     Resume: pmtu 16436 hops 1 back 64 
----------------------------------------------------------------------------------------
lixi@lixi-desktop:~$ tracepath 210.45.74.25 -l 1500
 1:  lixi-desktop.local (210.45.74.25)                      0.064ms reached
     Resume: pmtu 1500 hops 1 back 64 
----------------------------------------------------------------------------------------
*/
                lixiprintf ("MTU is too large!\n");
                goto restart;
            }
            if (res == 0)

/*
找到主机或者有不可恢复的错误都结束
*/
                goto done;
            if (res > 0)
                break;
        }

        if (res < 0)

/*
没有回音
*/
            printf ("%2d:  no reply\n", ttl);
    }

/*
31跳之内没有到达目的主机
*/
    printf ("     Too many hops: pmtu %d\n", mtu);
  done:
    printf ("     Resume: pmtu %d ", mtu);
    if (hops_to >= 0)
        printf ("hops %d ", hops_to);
    if (hops_from >= 0)
        printf ("back %d ", hops_from);
    printf ("\n");
    exit (0);
}
