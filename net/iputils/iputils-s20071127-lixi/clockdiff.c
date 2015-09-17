#include <time.h>
#include <sys/types.h>
#include <sys/param.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
#include <sys/timex.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#define TSPTYPES
#include <protocols/timed.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <linux/types.h>

void usage (void) __attribute__ ((noreturn));


/*
 * Checksum routine for Internet Protocol family headers.
 *
 * This routine is very heavily used in the network
 * code and should be modified for each CPU to be as fast as possible.
 *
 * This implementation is TAHOE version.
 */

#undef    ADDCARRY
#define ADDCARRY(sum) { \
    if (sum & 0xffff0000) {    \
        sum &= 0xffff; \
        sum++; \
    } \
}

int in_cksum (u_short * addr, int len)
{
    union word
    {
        char c[2];
        u_short s;
    } u;

    int sum = 0;

    while (len > 0)
    {
        /*
         * add by words.
         */
        while ((len -= 2) >= 0)
        {
            if ((unsigned long) addr & 0x1)
            {
                /* word is not aligned */
                u.c[0] = *(char *) addr;
                u.c[1] = *((char *) addr + 1);
                sum += u.s;
                addr++;
            }
            else
                sum += *addr++;
            ADDCARRY (sum);
        }
        if (len == -1)
            /*
             * Odd number of bytes.
             */
            u.c[0] = *(u_char *) addr;
    }
    if (len == -1)
    {
        /* The last mbuf has odd # of bytes. Follow the
           standard (the odd byte is shifted left by 8 bits) */
        u.c[1] = 0;
        sum += u.s;
        ADDCARRY (sum);
    }
    return (~sum & 0xffff);
}

#define ON        1
#define OFF        0

#define RANGE        1            /* best expected round-trip time, ms */
#define MSGS         50
#define TRIALS        10

#define GOOD        0
#define UNREACHABLE    2
#define NONSTDTIME    3
#define HOSTDOWN     0x7fffffff


int interactive = 0;

int id;

int sock;

int sock_raw;

struct sockaddr_in server;

int ip_opt_len = 0;

#define BIASP         43199999
#define BIASN        -43200000
#define MODULO         86400000
#define PROCESSING_TIME    0        /* ms. to reduce error in measurement */

#define PACKET_IN    1024

int measure_delta;

int measure_delta1;

static u_short seqno, seqno0, acked;

long rtt = 1000;

long min_rtt;

long rtt_sigma = 0;

#define lixiprintf printf

/*
 * Measures the differences between machines' clocks using
 * ICMP timestamp messages.
 */
int measure (struct sockaddr_in *addr)
{

/*
没有使用IP时间戳选项测量而使用ICMP时间戳
*/
    socklen_t length;

    int msgcount;

    int cc, count;

    fd_set ready;

    long sendtime, recvtime, histime, histime1;

    long min1, min2, diff;

    long delta1, delta2;

    struct timeval tv1, tout;

    u_char packet[PACKET_IN], opacket[64];

    struct icmphdr *icp = (struct icmphdr *) packet;

    struct icmphdr *oicp = (struct icmphdr *) opacket;

    struct iphdr *ip = (struct iphdr *) packet;

    min1 = min2 = 0x7fffffff;
    min_rtt = 0x7fffffff;
    measure_delta = HOSTDOWN;
    measure_delta1 = HOSTDOWN;

/* empties the icmp input queue */
    FD_ZERO (&ready);

  empty:
    tout.tv_sec = tout.tv_usec = 0;
    FD_SET (sock_raw, &ready);
    if (select (FD_SETSIZE, &ready, (fd_set *) 0, (fd_set *) 0, &tout))
    {
        length = sizeof (struct sockaddr_in);
        cc = recvfrom (sock_raw, (char *) packet, PACKET_IN, 0, (struct sockaddr *) NULL, &length);
        if (cc < 0)
            return -1;
        goto empty;
    }

    /*
     * To measure the difference, select MSGS messages whose round-trip
     * time is smaller than RANGE if ckrange is 1, otherwise simply
     * select MSGS messages regardless of round-trip transmission time.
     * Choose the smallest transmission time in each of the two directions.
     * Use these two latter quantities to compute the delta between
     * the two clocks.
     */

    length = sizeof (struct sockaddr_in);
    oicp->type = ICMP_TIMESTAMP;

/*
类型：时间戳请求
*/
    oicp->code = 0;

/*
代码：0
*/
    oicp->checksum = 0;

/*
检验和
*/
    oicp->un.echo.id = id;

/*
标识符：当前进程ID
*/
    ((__u32 *) (oicp + 1))[0] = 0;
    ((__u32 *) (oicp + 1))[1] = 0;
    ((__u32 *) (oicp + 1))[2] = 0;

/*
((__u32*)(oicp+1))[n]的地址:
oicp + 1*sizeof(oicp) + n*sizeof(__u32*)
将发起时间戳、接受时间戳、传送时间戳设置为0
*/
    FD_ZERO (&ready);
    msgcount = 0;

    acked = seqno = seqno0 = 0;

    for (msgcount = 0; msgcount < MSGS;)
    {

/*
发送MSGS（50）个？
*/

        /*
         * If no answer is received for TRIALS consecutive times,
         * the machine is assumed to be down
         */
        if (seqno - acked > TRIALS)

/*
如果发送的报文数比接受到的报文数多太多，则认为服务器荡掉了
*/
            return HOSTDOWN;

        oicp->un.echo.sequence = ++seqno;

/*
设置序列号
*/
        oicp->checksum = 0;

        (void) gettimeofday (&tv1, (struct timezone *) 0);
        *(__u32 *) (oicp + 1) = htonl ((tv1.tv_sec % (24 * 60 * 60)) * 1000 + tv1.tv_usec / 1000);

/*
设置发起时间戳
*/
        oicp->checksum = in_cksum ((u_short *) oicp, sizeof (*oicp) + 12);

/*
计算校验和，报文的大小一共是sizeof(*oicp) + 12字节
每个时间戳4个字节，三个时间戳一共有12字节。
*/
        count = sendto (sock_raw, (char *) opacket, sizeof (*oicp) + 12, 0,
                        (struct sockaddr *) addr, sizeof (struct sockaddr_in));

/*
发送ICMP报文
*/
        if (count < 0)

/*
发送失败，返回
*/
            return UNREACHABLE;

        for (;;)
        {
            FD_ZERO (&ready);

/*
清楚描述字集合中的所有bit位
*/
            FD_SET (sock_raw, &ready);

/*
设置描述字集合中对应sock_raw的位
*/
            {
                long tmo = rtt + rtt_sigma;

/*
设置超时时间，用的就是Jacobson/Karels算法
*/
                tout.tv_sec = tmo / 1000;
                tout.tv_usec = (tmo - (tmo / 1000) * 1000) * 1000;
            }

            if ((count = select (FD_SETSIZE, &ready, (fd_set *) 0, (fd_set *) 0, &tout)) <= 0)

/*
select()等待超时，没有可读的数据，就会返回0
出错返回-1
返回正数：准备好描述字的数目
*/
                break;

            (void) gettimeofday (&tv1, (struct timezone *) 0);
            cc = recvfrom (sock_raw, (char *) packet, PACKET_IN, 0, (struct sockaddr *) NULL, &length);

            if (cc < 0)
                return (-1);

            icp = (struct icmphdr *) (packet + (ip->ihl << 2));
            if (icp->type == ICMP_TIMESTAMPREPLY &&
                icp->un.echo.id == id && icp->un.echo.sequence >= seqno0 && icp->un.echo.sequence <= seqno)
            {

/*
类型：时间戳应答
标识符：当前进程ID
序列号：不小于seqno0，不大于seqno
*/
                if (acked < icp->un.echo.sequence)
                    acked = icp->un.echo.sequence;

                recvtime = (tv1.tv_sec % (24 * 60 * 60)) * 1000 + tv1.tv_usec / 1000;

/*
接到回复报文时间是：由于sock_raw描述字准备好读而导致select()函数返回时的系统时间
*/
                sendtime = ntohl (*(__u32 *) (icp + 1));

/*
发送时间是发起时间戳中记录的时间
*/
                diff = recvtime - sendtime;
                /*
                 * diff can be less than 0 aroud midnight
                 */
                if (diff < 0)

/*
午夜时，由于系统时间回绕而导致的时间差小于0
*/
                    continue;
                rtt = (rtt * 3 + diff) / 4;
                rtt_sigma = (rtt_sigma * 3 + abs (diff - rtt)) / 4;

/*
对RTT的预测？
*/
                msgcount++;
                histime = ntohl (((__u32 *) (icp + 1))[1]);
                histime1 = ntohl (((__u32 *) (icp + 1))[2]);
                /*
                 * a hosts using a time format different from
                 * ms. since midnight UT (as per RFC792) should
                 * set the high order bit of the 32-bit time
                 * value it transmits.
                 */
                if ((histime & 0x80000000) != 0)

/*
时间戳的取值一般为自UTC午夜（1900年1月1日午夜）开始计的毫秒数。
如果路由器不使用这种格式，它就可以插入任何它使用的时间表示格式，但是必须打开时间戳中的高位以表示为非标准值。
*/
                    return NONSTDTIME;

                if (interactive)
                {
                    printf (".");
                    fflush (stdout);
                }

                delta1 = histime - sendtime;
                /*
                 * Handles wrap-around to avoid that around
                 * midnight small time differences appear
                 * enormous. However, the two machine's clocks
                 * must be within 12 hours from each other.
                 */
                if (delta1 < BIASN)
                    delta1 += MODULO;
                else if (delta1 > BIASP)
                    delta1 -= MODULO;

/*
解决由于系统时间回绕而造成的时间相差过大的问题，这里有一个基本的假设：
两台主机的系统时间相差不超过12小时，否则计算出错
*/

                delta2 = recvtime - histime;
                if (delta2 < BIASN)
                    delta2 += MODULO;
                else if (delta2 > BIASP)
                    delta2 -= MODULO;

/*
至此，可以认为delta1和delta2 都是一个比较正常的值。
delta1：接受时间戳减去发起时间戳
delta2：接到回复报文时间减传送时间戳
min1：几次传送中delta1的最小值
min2：几次传送中delta2的最小值
min_rtt:几次传送中delta1+delta2的最小值
PROCESSING_TIME：处理消耗的时间，这里设置为0，认为处理消耗时间可以忽略
diff:本次收发的RTT
*/
                if (delta1 < min1)
                    min1 = delta1;
                if (delta2 < min2)
                    min2 = delta2;
                if (delta1 + delta2 < min_rtt)
                {
                    min_rtt = delta1 + delta2;
                    measure_delta1 = (delta1 - delta2) / 2 + PROCESSING_TIME;

/*
基于基本假设：
2. 当delta1 + delta2最小时，(delta1 - delta2)/2是对deltaT的最佳预测。
*/
                }
                if (diff < RANGE)
                {

/*
如果本次收发的RTT小于RANGE，则表明网络条件很好，RTT造成的不准确性很小，可以直接输出
*/
                    lixiprintf ("Good Network\n");
                    min1 = delta1;
                    min2 = delta2;
                    goto good_exit;
                }
            }
        }
    }

  good_exit:
    measure_delta = (min1 - min2) / 2 + PROCESSING_TIME;

/*
两台主机的系统时间相差detaT，即源主机的系统时间为T的时刻，目的主机的系统时间为T+detaT。
min1是对RTT/2+detaT的预测。
min2是对RTT/2-detaT的预测。
基于基本假设：
1. RTT中发送到目的主机的时间和返回源主机的时间基本相等都为RTT/2。
2. 当delta1最小时，是对RTT/2+detaT的最佳预测。同样，当delta2最小时，是对RTT/2-detaT的最佳预测。
3. 各主机从接受到报文到记录接受到报文时间，这两个时刻的时间间隔为可以忽略；即发送和接受报文的处理过程中所消耗的时间可以忽略。实际上PROCESSING_TIME正是用来消除由处理过程的时间造成的对于计算出来的系统时间差别的影响。
以上的几个假设决定了clockdiff预计出来的系统时间差别的不准确性。
*/
    return GOOD;
}

char *myname, *hisname;

int measure_opt (struct sockaddr_in *addr)
{

/*
使用IP时间戳选项测量
*/
    socklen_t length;

    int msgcount;

    int cc, count;

    fd_set ready;

    long sendtime, recvtime, histime, histime1;

    long min1, min2, diff;

    long delta1, delta2;

    struct timeval tv1, tout;

    u_char packet[PACKET_IN], opacket[64];

    struct icmphdr *icp = (struct icmphdr *) packet;

    struct icmphdr *oicp = (struct icmphdr *) opacket;

    struct iphdr *ip = (struct iphdr *) packet;

    min1 = min2 = 0x7fffffff;
    min_rtt = 0x7fffffff;
    measure_delta = HOSTDOWN;
    measure_delta1 = HOSTDOWN;

/* empties the icmp input queue */
    FD_ZERO (&ready);
  empty:
    tout.tv_sec = tout.tv_usec = 0;
    FD_SET (sock_raw, &ready);
    if (select (FD_SETSIZE, &ready, (fd_set *) 0, (fd_set *) 0, &tout))
    {
        length = sizeof (struct sockaddr_in);
        cc = recvfrom (sock_raw, (char *) packet, PACKET_IN, 0, (struct sockaddr *) NULL, &length);
        if (cc < 0)
            return -1;
        goto empty;
    }

    /*
     * To measure the difference, select MSGS messages whose round-trip
     * time is smaller than RANGE if ckrange is 1, otherwise simply
     * select MSGS messages regardless of round-trip transmission time.
     * Choose the smallest transmission time in each of the two directions.
     * Use these two latter quantities to compute the delta between
     * the two clocks.
     */

    length = sizeof (struct sockaddr_in);
    oicp->type = ICMP_ECHO;
    oicp->code = 0;
    oicp->checksum = 0;
    oicp->un.echo.id = id;
    ((__u32 *) (oicp + 1))[0] = 0;
    ((__u32 *) (oicp + 1))[1] = 0;
    ((__u32 *) (oicp + 1))[2] = 0;

    FD_ZERO (&ready);
    msgcount = 0;

    acked = seqno = seqno0 = 0;

    for (msgcount = 0; msgcount < MSGS;)
    {

        /*
         * If no answer is received for TRIALS consecutive times,
         * the machine is assumed to be down
         */
        if (seqno - acked > TRIALS)
        {
            errno = EHOSTDOWN;
            return HOSTDOWN;
        }
        oicp->un.echo.sequence = ++seqno;
        oicp->checksum = 0;

        gettimeofday (&tv1, NULL);
        ((__u32 *) (oicp + 1))[0] = htonl ((tv1.tv_sec % (24 * 60 * 60)) * 1000 + tv1.tv_usec / 1000);
        oicp->checksum = in_cksum ((u_short *) oicp, sizeof (*oicp) + 12);

        count = sendto (sock_raw, (char *) opacket, sizeof (*oicp) + 12, 0,
                        (struct sockaddr *) addr, sizeof (struct sockaddr_in));

        if (count < 0)
        {
            errno = EHOSTUNREACH;
            return UNREACHABLE;
        }

        for (;;)
        {
            FD_ZERO (&ready);
            FD_SET (sock_raw, &ready);
            {
                long tmo = rtt + rtt_sigma;

                tout.tv_sec = tmo / 1000;
                tout.tv_usec = (tmo - (tmo / 1000) * 1000) * 1000;
            }

            if ((count = select (FD_SETSIZE, &ready, (fd_set *) 0, (fd_set *) 0, &tout)) <= 0)
                break;

            (void) gettimeofday (&tv1, (struct timezone *) 0);
            cc = recvfrom (sock_raw, (char *) packet, PACKET_IN, 0, (struct sockaddr *) NULL, &length);

            if (cc < 0)
                return (-1);

            icp = (struct icmphdr *) (packet + (ip->ihl << 2));
            if (icp->type == ICMP_ECHOREPLY &&
                packet[20] == IPOPT_TIMESTAMP &&
                icp->un.echo.id == id && icp->un.echo.sequence >= seqno0 && icp->un.echo.sequence <= seqno)
            {
                int i;

                __u8 *opt = packet + 20;

/*
下面这一段是与measure()不一样的
*/
                if (acked < icp->un.echo.sequence)
                    acked = icp->un.echo.sequence;
                if ((opt[3] & 0xF) != IPOPT_TS_PRESPEC)
                {

/*
IPOPT_TS_PRESPEC：收集来自预定的网络段的时间戳和地址
*/
                    fprintf (stderr, "Wrong timestamp %d\n", opt[3] & 0xF);
                    return NONSTDTIME;
                }
                if (opt[3] >> 4)
                {

/*
IP时间戳选项的溢出字段
溢出字段不为0，表明由于没有空间而不能增加时间戳选项。
如果ip_opt_len是4+3*8的时候溢出1个时间戳，则如果ip_opt_len为4+4*8就可以不溢出了
如果溢出超过两个，无论ip_opt_len为何值都会溢出。
*/
                    if ((opt[3] >> 4) != 1 || ip_opt_len != 4 + 3 * 8)
                        fprintf (stderr, "Overflow %d hops\n", opt[3] >> 4);
                }
                sendtime = recvtime = histime = histime1 = 0;
                for (i = 0; i < (opt[2] - 5) / 8; i++)
                {
                    __u32 *timep = (__u32 *) (opt + 4 + i * 8 + 4);

                    __u32 t = ntohl (*timep);

                    if (t & 0x80000000)

/*
时间不是标准形式
*/
                        return NONSTDTIME;

                    if (i == 0)
                        sendtime = t;
                    if (i == 1)
                        histime = histime1 = t;
                    if (i == 2)
                    {
                        if (ip_opt_len == 4 + 4 * 8)
                            histime1 = t;
                        else
                            recvtime = t;
                    }
                    if (i == 3)
                        recvtime = t;
                }

/*
参考时间戳的形式：
本地主机IP  时间戳#1  目的主机IP  时间戳#2  本地主机IP  时间戳#3
本地主机IP  时间戳#1  目的主机IP  时间戳#2  目的主机IP  时间戳#3  本地主机IP  时间戳#4
*/

                if (!(sendtime & histime & histime1 & recvtime))
                {

/*
因为高几位（例如年份）总是不为0且相等的，所以不用担心在时间戳不出错的的时候几个时间没有都为1的bit位
*/
                    fprintf (stderr, "wrong timestamps\n");
                    return -1;
                }

/*
下面这一段是与measure()一样
*/
                diff = recvtime - sendtime;
                /*
                 * diff can be less than 0 aroud midnight
                 */
                if (diff < 0)
                    continue;
                rtt = (rtt * 3 + diff) / 4;
                rtt_sigma = (rtt_sigma * 3 + abs (diff - rtt)) / 4;
                msgcount++;

                if (interactive)
                {
                    printf (".");
                    fflush (stdout);
                }

                delta1 = histime - sendtime;
                /*
                 * Handles wrap-around to avoid that around
                 * midnight small time differences appear
                 * enormous. However, the two machine's clocks
                 * must be within 12 hours from each other.
                 */
                if (delta1 < BIASN)
                    delta1 += MODULO;
                else if (delta1 > BIASP)
                    delta1 -= MODULO;

                delta2 = recvtime - histime1;
                if (delta2 < BIASN)
                    delta2 += MODULO;
                else if (delta2 > BIASP)
                    delta2 -= MODULO;

                if (delta1 < min1)
                    min1 = delta1;
                if (delta2 < min2)
                    min2 = delta2;
                if (delta1 + delta2 < min_rtt)
                {
                    min_rtt = delta1 + delta2;
                    measure_delta1 = (delta1 - delta2) / 2 + PROCESSING_TIME;
                }
                if (diff < RANGE)
                {
                    min1 = delta1;
                    min2 = delta2;
                    goto good_exit;
                }
            }
        }
    }

  good_exit:
    measure_delta = (min1 - min2) / 2 + PROCESSING_TIME;
    return GOOD;
}


/*
 * Clockdiff computes the difference between the time of the machine on
 * which it is called and the time of the machines given as argument.
 * The time differences measured by clockdiff are obtained using a sequence
 * of ICMP TSTAMP messages which are returned to the sender by the IP module
 * in the remote machine.
 * In order to compare clocks of machines in different time zones, the time
 * is transmitted (as a 32-bit value) in milliseconds since midnight UT.
 * If a hosts uses a different time format, it should set the high order
 * bit of the 32-bit quantity it transmits.
 * However, VMS apparently transmits the time in milliseconds since midnight
 * local time (rather than GMT) without setting the high order bit.
 * Furthermore, it does not understand daylight-saving time.  This makes
 * clockdiff behaving inconsistently with hosts running VMS.
 *
 * In order to reduce the sensitivity to the variance of message transmission
 * time, clockdiff sends a sequence of messages.  Yet, measures between
 * two `distant' hosts can be affected by a small error. The error can, however,
 * be reduced by increasing the number of messages sent in each measurement.
 */

void usage ()
{
    fprintf (stderr, "Usage: clockdiff [-o] <host>\n");
    exit (1);
}


int main (int argc, char *argv[])
{
    int measure_status;

    struct hostent *hp;

    char hostname[MAXHOSTNAMELEN];

    int s_errno = 0;

    if (argc < 2)
    {

/*
少于两个参数是不正确的，程序至少有两个参数，即clockdiff hostName的形式
*/
        if (setuid (getuid ()))
        {
            perror ("clockdiff: setuid");
            exit (-1);
        }
        usage ();
    }

    sock_raw = socket (AF_INET, SOCK_RAW, IPPROTO_ICMP);

/*
取得ICMP类型的套接字
*/
    s_errno = errno;

    if (setuid (getuid ()))
    {
        perror ("clockdiff: setuid");
        exit (-1);
    }

    if (argc == 3)
    {

/*
程序的参数个数不是三个就是两个
如果是三个参数，则参数的形式是
clockdiff -o hostName或者
clockdiff -o1 hostName
*/
        if (strcmp (argv[1], "-o") == 0)
        {
            ip_opt_len = 4 + 4 * 8;
            argv++;

/*
argv++的原因是让参数为3个和两个的两种情况下，argv[1]都指向用户输入的目的主机名
*/
        }
        else if (strcmp (argv[1], "-o1") == 0)
        {
            ip_opt_len = 4 + 3 * 8;
            argv++;
        }
        else
            usage ();
    }
    else if (argc != 2)
        usage ();

    if (sock_raw < 0)
    {

/*
没有成功取得套接字
*/
        errno = s_errno;
        perror ("clockdiff: socket");
        exit (1);
    }

    if (isatty (fileno (stdin)) && isatty (fileno (stdout)))
        interactive = 1;

/*
int isatty(int desc):returns 1 if desc is an open descriptor connected to a terminal and 0 else. 
*/

    id = getpid ();

    (void) gethostname (hostname, sizeof (hostname));

/*
gethostname()取得主机名，存入hostname数组中
*/

    hp = gethostbyname (hostname);

/*
The gethostbyname() function returns a structure of type hostent for the given host name. 
*/
    if (hp == NULL)
    {
        fprintf (stderr, "clockdiff: %s: my host not found\n", hostname);
        exit (1);
    }
    myname = strdup (hp->h_name);

/*
The strdup() function returns a pointer to a new string which is a duplicate of the string s.
复制了一份主机名字
*/
    hp = gethostbyname (argv[1]);

/*
取得目的主机
*/
    if (hp == NULL)
    {
        fprintf (stderr, "clockdiff: %s: host not found\n", argv[1]);
        exit (1);
    }
    hisname = strdup (hp->h_name);

    memset (&server, 0, sizeof (server));
    server.sin_family = hp->h_addrtype;
    lixiprintf ("h_addrtype:%d, and note that AF_INET:%d  AF_INET6:%d\n", hp->h_addrtype, AF_INET, AF_INET6);
    memcpy (&(server.sin_addr.s_addr), hp->h_addr, 4);

    if (connect (sock_raw, (struct sockaddr *) &server, sizeof (server)) == -1)
    {

/*
连接
*/
        perror ("connect");
        exit (1);
    }
    if (ip_opt_len)
    {
        struct sockaddr_in myaddr;

        socklen_t addrlen = sizeof (myaddr);

        unsigned char rspace[ip_opt_len];

        memset (rspace, 0, sizeof (rspace));
        rspace[0] = IPOPT_TIMESTAMP;

/*
IPOPT_TIMESTAMP：0x44
*/
        rspace[1] = ip_opt_len;
        rspace[2] = 5;
        rspace[3] = IPOPT_TS_PRESPEC;

/*
参考IP时间戳选项的格式图就能比较好地分析上面的设置了
*/

        if (getsockname (sock_raw, (struct sockaddr *) &myaddr, &addrlen) == -1)
        {

/*
赋给source当前socket的名字，详见ping.c
*/
            perror ("getsockname");
            exit (1);
        }
        ((__u32 *) (rspace + 4))[0 * 2] = myaddr.sin_addr.s_addr;
        ((__u32 *) (rspace + 4))[1 * 2] = server.sin_addr.s_addr;
        ((__u32 *) (rspace + 4))[2 * 2] = myaddr.sin_addr.s_addr;
        if (ip_opt_len == 4 + 4 * 8)
        {
            ((__u32 *) (rspace + 4))[2 * 2] = server.sin_addr.s_addr;
            ((__u32 *) (rspace + 4))[3 * 2] = myaddr.sin_addr.s_addr;
        }

/*
设置时间戳选项中的地址。
形式如：
本地主机IP  时间戳  目的主机IP  时间戳  本地主机IP  时间戳
本地主机IP  时间戳  目的主机IP  时间戳  目的主机IP  时间戳  本地主机IP  时间戳
*/
        lixiprintf ("width:%dbytes\n",
                    (int) &(((__u32 *) (rspace + 4))[1 * 2]) - (int) &(((__u32 *) (rspace + 4))[0 * 2]));
        lixiprintf ("interval:%dbytes\n", (int) &(((__u32 *) (rspace + 4))[0 * 2]) - (int) &(rspace[3]));
        lixiprintf ("sizeof(s_addr):%dbytes\n", (int) sizeof (myaddr.sin_addr.s_addr));

/*
通过上述测试得到相邻两个相距8个字节
(__u32*)(rspace+4))[n*2]的地址计算方法为rspace + 4*sizeof(rspace)+ n*2*sizeof(__u32)
其中sizeof(rspace)为1，sizeof(__u32)为4，所以有：
从rspace[4]开始存储4字节的myaddr.sin_addr.s_addr
从rspace[12]开始存储4字节的server.sin_addr.s_addr
若ip_opt_len为32，则从rspace[20]开始存储4字节的myaddr.sin_addr.s_addr;
若ip_opt_len为36，则从rspace[20]开始存储4字节的server.sin_addr.s_addr，并则从rspace[28]开始存储4字节的myaddr.sin_addr.s_addr
*/
        if (setsockopt (sock_raw, IPPROTO_IP, IP_OPTIONS, rspace, ip_opt_len) < 0)
        {

/*
设置IP选项
*/
            perror ("ping: IP_OPTIONS (fallback to icmp tstamps)");
            ip_opt_len = 0;
        }
    }

    nice (-16);

/*
int nice(int incr)：The nice() function shall add the value of incr to the nice value of the calling process. 
*/
    if ((measure_status = (ip_opt_len ? measure_opt : measure) (&server)) < 0)
    {

/*
如果ip_opt_len为0，则执行measure(&server)；否则执行measure_opt(&server)
返回值存储到measure_status里
为什么要写的如此简练，以至于让人头大
*/
        if (errno)
            perror ("measure");
        else
            fprintf (stderr, "measure: unknown failure\n");
        exit (1);
    }

    switch (measure_status)
    {
        case HOSTDOWN:
            fprintf (stderr, "%s is down\n", hisname);
            exit (1);
        case NONSTDTIME:
            fprintf (stderr, "%s time transmitted in a non-standard format\n", hisname);
            exit (1);
        case UNREACHABLE:
            fprintf (stderr, "%s is unreachable\n", hisname);
            exit (1);
        default:
            break;
    }


    {
        time_t now = time (NULL);

        if (interactive)
            printf ("\nhost=%s rtt=%ld(%ld)ms/%ldms delta=%dms/%dms %s", hisname,
                    rtt, rtt_sigma, min_rtt, measure_delta, measure_delta1, ctime (&now));
        else
            printf ("%ld %d %d\n", now, measure_delta, measure_delta1);
    }
    exit (0);
}
