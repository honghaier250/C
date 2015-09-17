#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <linux/sockios.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/uio.h>
#include <sys/poll.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/types.h>
#include <linux/errqueue.h>

#include "SNAPSHOT.h"

#define    DEFDATALEN    (64 - 8)    /* default data length */

#define    MAXWAIT        10            /* max seconds to wait for response */
#define MININTERVAL    10            /* Minimal interpacket gap */
#define MINUSERINTERVAL    200        /* Minimal allowed interval for non-root */

#define SCHINT(a)    (((a) <= MININTERVAL) ? MININTERVAL : (a))

#define    A(bit)        rcvd_tbl[(bit)>>3]    /* identify byte in array */
#define    B(bit)        (1 << ((bit) & 0x07))    /* identify bit in byte */
#define    SET(bit)    (A(bit) |= B(bit))
#define    CLR(bit)    (A(bit) &= (~B(bit)))
#define    TST(bit)    (A(bit) & B(bit))
#define lixiprintf    printf

/* various options */
extern int options;

#define    F_FLOOD        0x001

/*
-f    洪泛模式以很高的速度发送会送分组来加强网络测试。如果不仔细使用，这可能导致严重问题
设置interval就可以了
*/
#define    F_INTERVAL    0x002

/*
-i <等待间隔>    告诉这个使用程序两次传输之间要等待多长时间，默认为1秒，不能和-f参数一起使用
设置F_INTERVAL就可以了
*/
#define    F_NUMERIC    0x004

/*
-n    只有数字形式的输出，不查找DNS主机以节省时间
只要设置了F_NUMERIC，就不用调用gethostbyaddr来查询DNS主机名了
用gethostbyaddr的由查询目的主机的IP地址
*/
#define    F_PINGFILLED    0x008

/*
-p <模式>    允许为传输的回显报文中包含的内容指定字节模式。这对于诊断与数据有关的网络问题可能很有用。数据采用16进制，例如“-p ff”可将传输的报文填充为全1

    if (!(options & F_PINGFILLED)) {
        int i;
        u_char *p = outpack+8;

        Do not forget about case of small datalen,
         fill timestamp area too!
         
        for (i = 0; i < datalen; ++i)
            *p++ = i;
    }

*/
#define    F_QUIET        0x010

/*
-q    静默输出，只在程序执行的开始和结束显示摘要行，不显示整队每个单独报文的行
只需要在需要输出的时候查询F_QUIET是否设置就可以了，或者如gather_statistics()函数中，在打印信息之前判断，如果F_QUIET设置了就返回。
*/
#define    F_RROUTE    0x020

/*
-R    使得程序记录路径IP选项，以便可以显示ICMP回显报文所采取的路由。IP头只有空间存储九个这样的路由信息。更好的办法是使用tracepath（traceroute）
*/
#define    F_SO_DEBUG    0x040

/*
-d    设置socket中的SO_DEBUG选项
*/
#define    F_SO_DONTROUTE    0x080

/*
-r    不通过路由而通过直接连接传送到目的主机，可以用来ping一个不需经过路由就可到达的本地主机，否则就会出现主机不可达的错误
*/
#define    F_VERBOSE    0x100

/*
-v    输出冗长信息
*/
#define    F_TIMESTAMP    0x200

/*
-T <timestamp option>    <timestamp option>可为tsonly (only times-tamps)，tsandaddr (timestamps and addresses)，或者 tsprespec host1 [host2 [host3[host 4]]] (timestamp preprecified hops)
*/
#define    F_FLOWINFO    0x200

/*
IPv6中才用到
*/
#define    F_SOURCEROUTE    0x400

/*
当ping hostName1 hostName2 .... hostNameN的时候会设置此选项
实际的目的主机是hostNameN，而前面的主机IP会被存储在IP选项里
暂时还弄不清楚做有什么作用
*/
#define    F_TCLASS    0x400

/*
IPv6中才用到
*/
#define    F_FLOOD_POLL    0x800
#define    F_LATENCY    0x1000

/*
-U    
 Print  full  user-to-user  latency (the old behaviour). Normally
              ping prints network round trip time, which can be different f.e.
              due to DNS failures.
*/
#define    F_AUDIBLE    0x2000

/*
-a    可听见的ping,所谓可听见,不过是在ping.c文件的parse_reply的函数中,输出ASCII码'/a',beep一下
*/
#define    F_ADAPTIVE    0x4000

/*
-A    自适应的ping。调整报文间隔时间，使其适应于RTT....。对于非超级用户，最小的时间间隔为200毫秒，在一个RTT比较得下的网络中，这个模式和-f的洪泛模式基本相同
为了调整报文时间间隔,使用update_interval()函数来调整时间间隔
*/
#define    F_STRICTSOURCE    0x8000

/*
-B    不允许ping改变报文的源主机地址，该地址在ping开始运行的时候就已经指定了
为了指定源主机地址,ping.c使用函数bind()来把套接字和本地套接字地址绑定，即在已创建的套接字上加上本地套接字地址。
*/
#define F_NOLOOP    0x10000

/*
-L    禁止多播数据包的回环，只有在ping的目的主机是广播地址时才管用
为了这一点，只需要设置socket的IP_MULTICAST_LOOP选项
*/
#define F_TTL        0x20000

/*
-t <ttl>    设置报文的ttl（Time To Live）
*/

/*
 * MAX_DUP_CHK is the number of bits in received table, i.e. the maximum
 * number of received sequence numbers we can keep track of.
 */
#define    MAX_DUP_CHK    0x10000
extern int mx_dup_ck;

extern char rcvd_tbl[MAX_DUP_CHK / 8];


extern u_char outpack[];

extern int maxpacket;

extern int datalen;

extern char *hostname;

extern int uid;

extern int ident;                /* process id to identify our packets */

extern int sndbuf;

extern int ttl;

extern long npackets;            /* max packets to transmit */

extern long nreceived;            /* # of packets we got back */

extern long nrepeats;            /* number of duplicates */

extern long ntransmitted;        /* sequence # for outbound packets = #sent */

extern long nchecksum;            /* replies with bad checksum */

extern long nerrors;            /* icmp errors */

extern int interval;            /* interval between packets (msec) */

extern int preload;

extern int deadline;            /* time to die */

extern int lingertime;

extern struct timeval start_time, cur_time;

extern volatile int exiting;

extern volatile int status_snapshot;

extern int confirm;

extern int confirm_flag;

extern int working_recverr;

#ifndef MSG_CONFIRM
#define MSG_CONFIRM 0
#endif


/* timing */
extern int timing;                /* flag to do timing */

extern long tmin;                /* minimum round trip time */

extern long tmax;                /* maximum round trip time */

extern long long tsum;            /* sum of all times, for doing average */

extern long long tsum2;

extern int rtt;

extern __u16 acked;

extern int pipesize;

#define COMMON_OPTIONS \
case 'a': case 'U': case 'c': case 'd': \
case 'f': case 'i': case 'w': case 'l': \
case 'S': case 'n': case 'p': case 'q': \
case 'r': case 's': case 'v': case 'L': \
case 't': case 'A': case 'W': case 'B':

#define COMMON_OPTSTR "h?VQ:I:M:aUc:dfi:w:l:S:np:qrs:vLt:AW:B"


/*
 * tvsub --
 *    Subtract 2 timeval structs:  out = out - in.  Out is assumed to
 * be >= in.
 */
static inline void tvsub (struct timeval *out, struct timeval *in)
{
    if ((out->tv_usec -= in->tv_usec) < 0)
    {
        --out->tv_sec;
        out->tv_usec += 1000000;
    }
    out->tv_sec -= in->tv_sec;
}

static inline void set_signal (int signo, void (*handler) (int))
{
    struct sigaction sa;

    memset (&sa, 0, sizeof (sa));

    sa.sa_handler = (void (*)(int)) handler;
#ifdef SA_INTERRUPT
    sa.sa_flags = SA_INTERRUPT;
#endif

/*
设置中断处理结构sigaction
struct sigaction {
    void (*sa_handler)(int);
    void (*sa_sigaction)(int, siginfo_t *, void *);
    sigset_t sa_mask;
    int sa_flags;
    void (*sa_restorer)(void);
}
*/
    sigaction (signo, &sa, NULL);
}

extern int __schedule_exit (int next);

static inline int schedule_exit (int next)
{
    if (npackets && ntransmitted >= npackets && !deadline)

/*
如果设定了需要传多少个报文，且传输的报文比接受的报文要多，且没有设置时间限制
传输的报文比接受的报文要多则要调用__schedule_exit()函数设置闹钟，在最多2 * tmax时间内（如果没有成功发收过，没有tmax数据，则设置为lingertime）退出
*/
        next = __schedule_exit (next);
    return next;
}

static inline int in_flight (void)
{
    __u16 diff = (__u16) ntransmitted - acked;

    return (diff <= 0x7FFF) ? diff : ntransmitted - nreceived - nerrors;

/*
返回正在传送中的报文个数
有两种计算方法：
ntransmitted - acked;
和
ntransmitted-nreceived-nerrors;
如果acked曾经回绕过就不能用上一种方法了
*/
}

static inline void acknowledge (__u16 seq)
{
    __u16 diff = (__u16) ntransmitted - seq;

/*
发送和接受到的序列号码之差
pipesize是一个受preload和interval的双重限制的数，当preload比较小时，pipsize等于preload；否则如果preload很大，就变为interval的限制。
*/
    if (diff <= 0x7FFF)
    {
        if ((int) diff + 1 > pipesize)
            pipesize = (int) diff + 1;
        if ((__s16) (seq - acked) > 0 || (__u16) ntransmitted - acked > 0x7FFF)

/*
发生回绕或者来了一个后发报文的ACK
*/
            acked = seq;
    }

/*
超过0x7FFF怎么办？
*/
}

static inline void advance_ntransmitted (void)
{
    ntransmitted++;
    /* Invalidate acked, if 16 bit seq overflows. */
    if ((__u16) ntransmitted - acked > 0x7FFF)
        acked = (__u16) ntransmitted + 1;

/*
如果序列号超过16bit
则废止acked，即重新设置acked
acked比较容易发生回绕，而ntransmitted不大可能
*/
}


extern int send_probe (void);

extern int receive_error_msg (void);

extern int parse_reply (struct msghdr *msg, int len, void *addr, struct timeval *);

extern void install_filter (void);

extern int pinger (void);

extern void sock_setbufs (int icmp_sock, int alloc);

extern void setup (int icmp_sock);

extern void main_loop (int icmp_sock, __u8 * buf, int buflen) __attribute__ ((noreturn));

extern void finish (void) __attribute__ ((noreturn));

extern void status (void);

extern void common_options (int ch);

extern int gather_statistics (__u8 * ptr, int cc, __u16 seq, int hops, int csfailed, struct timeval *tv, char *from);
