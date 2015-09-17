
/*
   ping6和ping程序都要使用这个文件里定义的函数
*/
#include "ping_common.h"
#include <ctype.h>
#include <sched.h>

int options;

int sndbuf;

int ttl;

int rtt;

int rtt_addend;

__u16 acked;

int mx_dup_ck = MAX_DUP_CHK;

/*
MAX_DUP_CHK:65536
*/
char rcvd_tbl[MAX_DUP_CHK / 8];


/* counters */
long npackets;                    /* max packets to transmit */

long nreceived;                    /* # of packets we got back */

long nrepeats;                    /* number of duplicates */

long ntransmitted;                /* sequence # for outbound packets = #sent */

long nchecksum;                    /* replies with bad checksum */

long nerrors;                    /* icmp errors */

int interval = 1000;            /* interval between packets (msec) */

int preload;

int deadline = 0;                /* time to die */

int lingertime = MAXWAIT * 1000;

struct timeval start_time, cur_time;

volatile int exiting;

volatile int status_snapshot;

int confirm = 0;

/* Stupid workarounds for bugs/missing functionality in older linuces.
 * confirm_flag fixes refusing service of kernels without MSG_CONFIRM.
 * i.e. for linux-2.2 */

/*
   MSG_CONFIRM (Linux 2.3+ only)
   只有linux 2.3以上的内核版本才支持MSG_CONFIRM
   也就是设置了就会出错啊
*/
int confirm_flag = MSG_CONFIRM;

/* And this is workaround for bug in IP_RECVERR on raw sockets which is present
 * in linux-2.2.[0-19], linux-2.4.[0-7] */
int working_recverr;

/* timing */
int timing;                        /* flag to do timing */

long tmin = LONG_MAX;            /* minimum round trip time */

/*
LONG_MAX:2147483647
*/
long tmax;                        /* maximum round trip time */

/* Message for rpm maintainers: have _shame_. If you want
 * to fix something send the patch to me for sanity checking.
 * "sparcfix" patch is a complete non-sense, apparenly the person
 * prepared it was stoned.
 */
long long tsum;                    /* sum of all times, for doing average */

long long tsum2;

int pipesize = -1;

int datalen = DEFDATALEN;

char *hostname;

int uid;

int ident;                        /* process id to identify our packets */

static int screen_width = INT_MAX;

/* Fills all the outpack, excluding ICMP header, but _including_
 * timestamp area with supplied pattern.
 */
static void fill (char *patp)
{
    int ii, jj, kk;

    int pat[16];

    char *cp;

    u_char *bp = outpack + 8;

    for (cp = patp; *cp; cp++)
    {
        if (!isxdigit (*cp))
        {
            fprintf (stderr, "ping: patterns must be specified as hex digits.\n");
            exit (2);
        }
    }
    ii = sscanf (patp,
            "%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x",
            &pat[0], &pat[1], &pat[2], &pat[3], &pat[4], &pat[5], &pat[6],
            &pat[7], &pat[8], &pat[9], &pat[10], &pat[11], &pat[12], &pat[13], &pat[14], &pat[15]);

    if (ii > 0)
    {
        for (kk = 0; kk <= maxpacket - (8 + ii); kk += ii)
            for (jj = 0; jj < ii; ++jj)
                bp[jj + kk] = pat[jj];
    }
    if (!(options & F_QUIET))
    {
        printf ("PATTERN: 0x");
        for (jj = 0; jj < ii; ++jj)
            printf ("%02x", bp[jj] & 0xFF);
        printf ("\n");
    }
}

void common_options (int ch)
{

    /* 处理参数 */
    switch (ch)
    {
        case 'a':
            options |= F_AUDIBLE;
            break;
        case 'A':
            options |= F_ADAPTIVE;
            break;
        case 'c':
            npackets = atoi (optarg);

            /*
               -c参数设定了发送ping命令的个数
               atoi将字符串转换为数字
            */
            if (npackets <= 0)
            {
                fprintf (stderr, "ping: bad number of packets to transmit.\n");
                exit (2);
            }
            break;
        case 'd':
            options |= F_SO_DEBUG;
            break;
        case 'f':
            options |= F_FLOOD;
            setbuf (stdout, (char *) NULL);
            break;
        case 'i':                /* wait between sending packets */
            {
                if (strchr (optarg, '.'))
                {
                    float t;

                    if (sscanf (optarg, "%f", &t) != 1)
                    {
                        fprintf (stderr, "ping: bad timing interval.\n");
                        exit (2);
                    }
                    interval = (int) (t * 1000);
                }
                else if (sscanf (optarg, "%d", &interval) == 1)
                {
                    interval *= 1000;
                }
                else
                {
                    fprintf (stderr, "ping: bad timing interval.\n");
                    exit (2);
                }

                if (interval < 0)
                {
                    fprintf (stderr, "ping: bad timing interval.\n");
                    exit (2);
                }
                options |= F_INTERVAL;
                break;
            }
        case 'w':
            deadline = atoi (optarg);

            /*
               -w参数设定时间期限
               不管已经发送和接到了多少包
               只要达到时间期限就结束ping的过程
            */
            if (deadline < 0)
            {
                fprintf (stderr, "ping: bad wait time.\n");
                exit (2);
            }
            break;
        case 'l':

            /*
               If  preload is specified, ping sends that many packets not wait-
               ing for reply.  Only the super-user may select preload more than
               3.
            */
            preload = atoi (optarg);
            if (preload <= 0)
            {
                fprintf (stderr, "ping: bad preload value, should be 1..%d\n", mx_dup_ck);
                exit (2);
            }
            if (preload > mx_dup_ck)
                preload = mx_dup_ck;
            if (uid && preload > 3)
            {
                fprintf (stderr, "ping: cannot set preload to value > 3\n");
                exit (2);
            }
            break;
        case 'S':
            sndbuf = atoi (optarg);
            if (sndbuf <= 0)
            {
                fprintf (stderr, "ping: bad sndbuf value.\n");
                exit (2);
            }
            break;
        case 'n':
            options |= F_NUMERIC;
            break;
        case 'p':                /* fill buffer with user pattern */
            options |= F_PINGFILLED;
            fill (optarg);
            break;
        case 'q':
            options |= F_QUIET;
            break;
        case 'r':
            options |= F_SO_DONTROUTE;
            break;
        case 's':                /* size of packet to send */
            datalen = atoi (optarg);
            if (datalen < 0)
            {
                fprintf (stderr, "ping: illegal negative packet size %d.\n", datalen);
                exit (2);
            }
            break;
        case 'v':
            options |= F_VERBOSE;
            break;
        case 'L':
            options |= F_NOLOOP;
            break;
        case 't':
            options |= F_TTL;
            ttl = atoi (optarg);
            if (ttl < 0 || ttl > 255)
            {
                fprintf (stderr, "ping: ttl %u out of range\n", ttl);
                exit (2);
            }
            break;
        case 'U':
            options |= F_LATENCY;
            break;
        case 'B':
            options |= F_STRICTSOURCE;
            break;
        case 'W':
            lingertime = atoi (optarg);
            if (lingertime < 0 || lingertime > INT_MAX / 1000000)
            {
                fprintf (stderr, "ping: bad linger time.\n");
                exit (2);
            }
            lingertime *= 1000;
            break;
        case 'V':
            printf ("ping utility, iputils-ss%s\n", SNAPSHOT);

            /*
               打印版本信息
            */
            exit (0);
        default:
            abort ();
    }
}

static void sigexit (int signo)
{
    lixiprintf ("SIGALRM\n");
    exiting = 1;
}

static void sigexit0 (int signo)
{
    lixiprintf ("SIGINT\n");
    exiting = 1;
}

static void sigstatus (int signo)
{
    lixiprintf ("SIGQUIT\n");
    status_snapshot = 1;
}


int __schedule_exit (int next)
{
    static unsigned long waittime;

    struct itimerval it;

    if (waittime)

        /*
           不会超时
        */
        return next;

    if (nreceived)
    {
        waittime = 2 * tmax;

        /*
           已经接到了第一个恢复，知道了RRT的大致情况
           两倍的最大RRT时间
        */
        if (waittime < 1000 * interval)

            /*
               设置为等待时间最长为1000*interval微妙
            */
            waittime = 1000 * interval;
    }
    else

        /*
           在没有得到任何回复之前，由于不知道RRT的大致情况，使用lingertime作为等待时间
        */
        waittime = lingertime * 1000;

    if (next < 0 || next < waittime / 1000)
        next = waittime / 1000;

    it.it_interval.tv_sec = 0;
    it.it_interval.tv_usec = 0;
    it.it_value.tv_sec = waittime / 1000000;
    it.it_value.tv_usec = waittime % 1000000;

    /*
       在等待waittime微秒之后，就会产生SIGALRM中断
       从而被sigexit()中断处理函数处理
       it_interval为下一个值
       it_value为当前值
    */
    lixiprintf ("setitimer\n");
    setitimer (ITIMER_REAL, &it, NULL);

    /*
       ITIMER_REAL
       decrements in real time, and delivers SIGALRM upon expiration.
       还可以设置为ITIMER_VIRTUAL，此时程序在运行时才运行，
       但是根据这里的需要，由于程序不运行的时候也在网络中传输？
    */
    return next;
}

static inline void update_interval (void)
{
    int est = rtt ? rtt / 8 : interval * 1000;

    /*
       初始的时候rtt变量值为0，故此将设置的interval来估计
    */
    interval = (est + rtt_addend + 500) / 1000;

    /*
       加上500是为了实现四舍五入
    */
    if (uid && interval < MINUSERINTERVAL)
        interval = MINUSERINTERVAL;

    /*
       如果不是超级用户，interval的值在任何情况下都不能小于MINUSERINTERVAL
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
int pinger (void)
{
    static int oom_count;

    static int tokens;

    int i;

    /* Have we already sent enough? If we have, return an arbitrary positive value. */
    if (exiting || (npackets && ntransmitted >= npackets && !deadline))
        return 1000;

    /*
       返回一个超大无比的数
       确保main_loop认为有时间来判断是否该退出
    */

    /* Check that packets < rate*time + preload */
    if (cur_time.tv_sec == 0)
    {

        /*
           第一次执行pinger()函数
           tokens赋值为interval*(preload-1)，也就是发送（提前发送的报文数－1）个报文所需要的时间。
           按照其含义，将tokens称为时间片。
        */
        gettimeofday (&cur_time, NULL);
        tokens = interval * (preload - 1);
        lixiprintf ("First Probe\n");
    }
    else
    {
        long ntokens;

        struct timeval tv;

        gettimeofday (&tv, NULL);
        ntokens = (tv.tv_sec - cur_time.tv_sec) * 1000 + (tv.tv_usec - cur_time.tv_usec) / 1000;

        /*
           当前时刻和上一次发报文的时刻的时间间隔
           如果一直在循环发送报文，而没有接受报文，则这个时间间隔肯定不足1毫秒，被忽略
        */
        if (!interval)
        {

            /*
               interval默认值是1000
               只有在-i选项下手动设置，或者被-f选项设置为0
            */
            /* Case of unlimited flood is special;
             * if we see no reply, they are limited to 100pps */
            if (ntokens < MININTERVAL && in_flight () >= preload)
                return MININTERVAL - ntokens;

            /*
               还没有等到相隔MININTERVAL毫秒时间，并且有preload个报文正在传送，那么就等待一会儿再发送报文。
               注意，只要一个条件不满足就会再发送一个报文。这样，如果正在传送的报文没有preload个，则会被迅速发送几个报文；并且即使正在传送的报文超过preload，MININTERVAL时限一过，就又会发送一个报文。这就是洪泛模式。
            */
        }
        ntokens += tokens;
        if (ntokens > interval * preload)

            /*
               分配的时间片不能使得发送超过preload-1个报文成为可能
               假设一种情况：如果由于处理接受的报文而很长时间没有接受报文，那么ntokens肯定很大（ntokens是上次发送时刻到现在时刻的时间间隔）。经过这个语句之后，ntokens设置为interval*preload。这样时间片tokens就为ntokens - interval，即interval*(preload-1)。与初始情况一样，分得这么多时间片之后，就开始连续发preload个报文。
            */
            ntokens = interval * preload;
        if (ntokens < interval)

            /*
               剩下的时间片不足一个间隔，则不再发送，开始接受报文
            */
            return interval - ntokens;

        cur_time = tv;
        tokens = ntokens - interval;
        lixiprintf ("Not First Probe, ntokens:%ld, tokens:%d\n", ntokens, tokens);
    }
    lixiprintf ("Send a Probe\n");
resend:
    i = send_probe ();

    if (i == 0)
    {

        /*
           传输成功
        */
        oom_count = 0;
        advance_ntransmitted ();
        if (!(options & F_QUIET) && (options & F_FLOOD))
        {

            /*
               在非静默输出且在洪泛模式下才输出一堆点来代表还有多少个报文没有被回答
            */
            /* Very silly, but without this output with
             * high preload or pipe size is very confusing. */
            if ((preload < screen_width && pipesize < screen_width) || in_flight () < screen_width)
                write (STDOUT_FILENO, ".", 1);

            /*
               一大堆判断用来判断是不是应该输出一个‘.’
               为的是保证输出的点不会超出一行，因为如果超出一行就不能通过输出‘\b’字符删除‘.’了。
            */

            /*
 *            lixiprintf("窗口的列(col)大小是：%d\n",screen_width);
              lixiprintf("preload：%d\n",preload);
              lixiprintf("pipesize：%d\n",pipesize);
              lixiprintf("in_flight()：%d\n",in_flight());
            */

        }
        return interval - tokens;

        /*
           如果tokens大于interval，即还有多余的时间片，返回的值是非正数。这样仍然不退出循环，继续连续发送报文。
        */
    }

    /* And handle various errors... */
    if (i > 0)
    {

        /*
           这种情况不可能出现
        */
        /* Apparently, it is some fatal bug. */
        abort ();

        /*
           The abort() function causes abnormal program termination unless the signal SIGABRT is caught and the signal handler does not return. If the abort() function causes program termination, all open streams are closed and flushed.
        */
    }
    else if (errno == ENOBUFS || errno == ENOMEM)
    {

        /*
           errno由send_probe()调用的函数sendmsg出错时设置
           ENOBUFS：The output queue for a network interface was full. This generally indicates that the interface has stopped sending, but may be caused by transient congestion. (Normally, this does not occur in Linux. Packets are just silently dropped when a device queue overflows.)
           ENOMEM：No memory available.
           表明网络接口的缓冲满，或者内存不够。处理方法：放慢发送的速度，只在TTL适应模式下管用
        */
        int nores_interval;

        /* Device queue overflow or OOM. Packet is not sent. */
        tokens = 0;
        /* Slowdown. This works only in adaptive mode (option -A) */
        rtt_addend += (rtt < 8 * 50000 ? rtt / 8 : 50000);
        if (options & F_ADAPTIVE)
            update_interval ();
        nores_interval = SCHINT (interval / 2);
        if (nores_interval > 500)
            nores_interval = 500;
        oom_count++;
        if (oom_count * nores_interval < lingertime)
            return nores_interval;
        i = 0;
        /* Fall to hard error. It is to avoid complete deadlock
         * on stuck output device even when dealine was not requested.
         * Expected timings are screwed up in any case, but we will
         * exit some day. :-) */
    }
    else if (errno == EAGAIN)
    {
        /* Socket buffer is full. */
        tokens += interval;
        return MININTERVAL;
    }
    else
    {
        if ((i = receive_error_msg ()) > 0)
        {

            /*
               receive_error_msg()表明由于网络错误或者本地错误造成的。
               ICMP错误报文
            */
            /* An ICMP error arrived. */
            tokens += interval;
            return MININTERVAL;
        }
        /* Compatibility with old linuces. */
        if (i == 0 && confirm_flag && errno == EINVAL)
        {

            /*
               receive_error_msg()函数判断，没有表明本地错误或者表明网络错误的ICMP错误报文
               由于receive_error_msg()中保存了errno的旧值，才调用有可能修改errno值的函数recvmsg()。所以EINVAL肯定是send_probe()中的函数sendmsg()设置的，它表明参数设置不对，说明linux的版本比较老于Linux 2.3
               我觉得这里有点问题：
               confirm也应该设置为0，这样send_probe()调用sendmsg()就不会再次出错，也就不用假装传输成功了，hiahia
               不过更加可能的是我的理解有问题...
            */
            lixiprintf ("Linux kernel vision is toooooOld\n");
            confirm_flag = 0;
            errno = 0;
        }
        if (!errno)
            goto resend;
    }
    /* Hard local error. Pretend we sent packet. */
    advance_ntransmitted ();

    if (i == 0 && !(options & F_QUIET))
    {
        if (options & F_FLOOD)
            write (STDOUT_FILENO, "E", 1);
        else
            perror ("ping: sendmsg");
    }
    tokens = 0;
    return SCHINT (interval);
}

/* Set socket buffers, "alloc" is an estimate of memory taken by single packet. */

void sock_setbufs (int icmp_sock, int alloc)
{
    int rcvbuf, hold;

    socklen_t tmplen = sizeof (hold);

    if (!sndbuf)
        sndbuf = alloc;
    setsockopt (icmp_sock, SOL_SOCKET, SO_SNDBUF, (char *) &sndbuf, sizeof (sndbuf));

    /*
       设置发送缓冲区大小
       SO_SNDBUF：Sets or gets the maximum socket send buffer in bytes.
    */

    rcvbuf = hold = alloc * preload;
    if (hold < 65536)
        hold = 65536;
    setsockopt (icmp_sock, SOL_SOCKET, SO_RCVBUF, (char *) &hold, sizeof (hold));

    /*
       设置接受缓冲区大小
       SO_RCVBUF：Sets or gets the maximum socket receive buffer in bytes.
    */
    if (getsockopt (icmp_sock, SOL_SOCKET, SO_RCVBUF, (char *) &hold, &tmplen) == 0)
    {

        /*
           设置不成功？
        */
        if (hold < rcvbuf)
            fprintf (stderr, "WARNING: probably, rcvbuf is not enough to hold preload.\n");
    }
}

/* Protocol independent setup and parameter checks. */

void setup (int icmp_sock)
{
    int hold;

    struct timeval tv;

    if ((options & F_FLOOD) && !(options & F_INTERVAL))

        /*
           -f参数不配合-i参数使用，则设置interval为0
           只有超级用户这样做才是合法的
        */
        interval = 0;

    if (uid && interval < MINUSERINTERVAL)
    {

        /*
           如果不是超级用户，-f参数必须要和-i参数配合使用，且interval不能设置得少于MINUSERINTERVAL
        */
        fprintf (stderr, "ping: cannot flood; minimal interval, allowed for user, is %dms\n", MINUSERINTERVAL);
        exit (2);
    }

    if (interval >= INT_MAX / preload)
    {

        /*
            INT_MAX:2147483647
            interval * preload不能溢出
        */
        fprintf (stderr, "ping: illegal preload and/or interval\n");
        exit (2);
    }

    hold = 1;
    if (options & F_SO_DEBUG)
        setsockopt (icmp_sock, SOL_SOCKET, SO_DEBUG, (char *) &hold, sizeof (hold));

    /*
       使能调试跟踪
       Enable socket debugging
    */
    if (options & F_SO_DONTROUTE)
        setsockopt (icmp_sock, SOL_SOCKET, SO_DONTROUTE, (char *) &hold, sizeof (hold));

    /*
       旁路路由表查询
       Don't send via a gateway, only send to directly connected hosts.
    */
#ifdef SO_TIMESTAMP
    if (!(options & F_LATENCY))
    {
        int on = 1;

        if (setsockopt (icmp_sock, SOL_SOCKET, SO_TIMESTAMP, &on, sizeof (on)))

            /*
               允许SO_TIMESTAMP接受控制信息
               SO_TIMESTAMP：Enable or disable the receiving of the SO_TIMESTAMP control message.
            */
            fprintf (stderr, "Warning: no SO_TIMESTAMP support, falling back to SIOCGSTAMP\n");

    }
#endif

    /* Set some SNDTIMEO to prevent blocking forever
     * on sends, when device is too slow or stalls. Just put limit
     * of one second, or "interval", if it is less.
  */
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if (interval < 1000)
    {
        tv.tv_sec = 0;
        tv.tv_usec = 1000 * SCHINT (interval);
    }
    setsockopt (icmp_sock, SOL_SOCKET, SO_SNDTIMEO, (char *) &tv, sizeof (tv));

    /*
       设置发送时间限制
       SO_SNDTIMEO：Specify the sending timeouts until reporting an error
    */
    /* Set RCVTIMEO to "interval". Note, it is just an optimization
     * allowing to avoid redundant poll(). */
    tv.tv_sec = SCHINT (interval) / 1000;

    /*
       SCHINT()确保了interval小于MININTERVAL时，等同于MININTERVAL
    */
    tv.tv_usec = 1000 * (SCHINT (interval) % 1000);
    if (setsockopt (icmp_sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof (tv)))

        /*
           设置接受时间限制
           SO_RCVTIMEO：Specify the receiving timeouts until reporting an error.
        */
        options |= F_FLOOD_POLL;

    if (!(options & F_PINGFILLED))
    {
        int i;

        u_char *p = outpack + 8;

        /* Do not forget about case of small datalen,
         * fill timestamp area too!
        */
        for (i = 0; i < datalen; ++i)
            *p++ = i;
    }

    ident = getpid () & 0xFFFF;

    set_signal (SIGINT, sigexit0);
    set_signal (SIGALRM, sigexit);
    set_signal (SIGQUIT, sigstatus);

    /*
       set_signal设置中断处理程序，处理中断
       set_signal定义在ping_common.h中
       sigexit
       <signal.h>中定义了SIGINT、SIGALRM、SIGQUIT等符号常量（symbolic constant）
       在控制台在运行的时候按Ctrl+C就会产生SIGINT中断
    */
    gettimeofday (&start_time, NULL);

    if (deadline)
    {
        struct itimerval it;

        it.it_interval.tv_sec = 0;
        it.it_interval.tv_usec = 0;
        it.it_value.tv_sec = deadline;
        it.it_value.tv_usec = 0;
        setitimer (ITIMER_REAL, &it, NULL);

        /*
           如果用-w参数设定了时间期限
           给程序设定计时器
           当deadline秒时产生SIGALRM中断
        */
    }

    if (isatty (STDOUT_FILENO))
    {

        /*
           判断STDOUT_FILENO是和一个终端相连
        */
        struct winsize w;

        if (ioctl (STDOUT_FILENO, TIOCGWINSZ, &w) != -1)
        {

            /*
               得到终端的窗口大小
            */
            if (w.ws_col > 0)
                screen_width = w.ws_col;
            lixiprintf ("窗口的列(col)大小是：%d\n", screen_width);
        }
    }
}

void main_loop (int icmp_sock, __u8 * packet, int packlen)
{

    /*主循环*/
    char addrbuf[128];

    char ans_data[4096];

    struct iovec iov;

    struct msghdr msg;

    struct cmsghdr *c;

    int cc;

    int next;

    int polling;

    iov.iov_base = (char *) packet;

    for (;;)
    {
        /* Check exit conditions. */
        if (exiting)
            break;

        /*
           发生SIGINT（例如ctrl+c） 和 SIGALARM（等待超时）中断，退出
           超时中断可能是由于某次报文等待回复的时间过长（比如说是ping一个不可达的地址）
           或者是在deadline设置过后，超出了设定的运行时间
        */ if (npackets && nreceived + nerrors >= npackets)

        /*
           已经接受到了npackets以上个回复，不管是不是有的回复有错误，都退出
           如果npackets没有设置，只能等待中断到来才能退出了
        */
        break;
        if (deadline && nerrors)

            /*
               如果有时间限制，且出过错，就不再执行程序了
            */
            break;
        /* Check for and do special actions. */
        if (status_snapshot)
            status ();

        /*
           发生SIGQUIT中断，打印出执行信息
        */
        /* Send probes scheduled to this time. */
        do
        {
            next = pinger ();

            /*
               第一次pinger不会有时间限制
            */
            lixiprintf ("<next> before schedule:%d\n", next);
            next = schedule_exit (next);
            lixiprintf ("<next> after schedule:%d\n", next);
        }
        while (next <= 0);
        lixiprintf ("out of while\n");

        /*
           如果时间很紧迫即next<=0
           为了尽快地发送探测包，不执行其他的部分
        */
        /* "next" is time to send next probe, if positive.
         * If next<=0 send now or as soon as possible. */

        /* Technical part. Looks wicked. Could be dropped,
         * if everyone used the newest kernel. :-)
         * Its purpose is:
         * 1. Provide intervals less than resolution of scheduler.
         *    Solution: spinning.
         * 2. Avoid use of poll(), when recvmsg() can provide
         *    timed waiting (SO_RCVTIMEO). */
        polling = 0;

        /*
           默认为阻塞的
        */
        if ((options & (F_ADAPTIVE | F_FLOOD_POLL)) || next < SCHINT (interval))
        {
            int recv_expected = in_flight ();

            /* If we are here, recvmsg() is unable to wait for
             * required timeout. */
            if (1000 * next <= 1000000 / (int) HZ)
            {
                /* Very short timeout... So, if we wait for
                 * something, we sleep for MININTERVAL.
                 * Otherwise, spin! */
                if (recv_expected)
                {
                    next = MININTERVAL;
                }
                else
                {
                    next = 0;
                    /* When spinning, no reasons to poll.
                     * Use nonblocking recvmsg() instead. */
                    polling = MSG_DONTWAIT;

                    /*
                       设置为不可阻断运作
                    */
                    /* But yield yet. */
                    sched_yield ();
                }
            }

            if (!polling && ((options & (F_ADAPTIVE | F_FLOOD_POLL)) || interval))
            {
                struct pollfd pset;

                pset.fd = icmp_sock;
                pset.events = POLLIN | POLLERR;
                pset.revents = 0;
                if (poll (&pset, 1, next) < 1 || !(pset.revents & (POLLIN | POLLERR)))
                    continue;
                polling = MSG_DONTWAIT;
            }
        }

        for (;;)
        {
            struct timeval *recv_timep = NULL;

            struct timeval recv_time;

            int not_ours = 0;    /* Raw socket can receive messages
                                  * destined to other running pings. */

            iov.iov_len = packlen;
            memset (&msg, 0, sizeof (msg));
            msg.msg_name = addrbuf;
            msg.msg_namelen = sizeof (addrbuf);
            msg.msg_iov = &iov;
            msg.msg_iovlen = 1;
            msg.msg_control = ans_data;
            msg.msg_controllen = sizeof (ans_data);

            /*
               给消息分配存储空间
            */

            cc = recvmsg (icmp_sock, &msg, polling);

            /*
               接受消息
            */
            polling = MSG_DONTWAIT;

            /*
               在一次调查之后，不管情况怎样，第二次都设置为不可阻断运作
            */
            if (cc < 0)
            {

                /*
                   在没有接受到信息的情况下，如果socket是阻塞的，则recvmsg一直等待。如果是非阻塞的，就会返回-1，且设置errno为EAGAIN
                   recvmsg返回负数
                */
                if (errno == EAGAIN || errno == EINTR)

                    /*
                       EINTR表明在数据准备好之前，接受者被中断。
                    */
                    break;
                if (!receive_error_msg ())
                {
                    if (errno)
                    {
                        perror ("ping: recvmsg");
                        break;
                    }
                    not_ours = 1;
                }
            }
            else
            {

#ifdef SO_TIMESTAMP
                for (c = CMSG_FIRSTHDR (&msg); c; c = CMSG_NXTHDR (&msg, c))
                {
                    if (c->cmsg_level != SOL_SOCKET || c->cmsg_type != SO_TIMESTAMP)
                        continue;
                    if (c->cmsg_len < CMSG_LEN (sizeof (struct timeval)))
                        continue;
                    recv_timep = (struct timeval *) CMSG_DATA (c);
                }
#endif

                if ((options & F_LATENCY) || recv_timep == NULL)
                {
                    if ((options & F_LATENCY) || ioctl (icmp_sock, SIOCGSTAMP, &recv_time))
                        gettimeofday (&recv_time, NULL);
                    recv_timep = &recv_time;
                }

                not_ours = parse_reply (&msg, cc, addrbuf, recv_timep);
            }

            /* See? ... someone runs another ping on this host. */
            if (not_ours)

                /*
                   多用户的主机有可能很多人同时使用主机
                   故此使用install_filter()函数来屏蔽
                */
                install_filter ();

            /* If nothing is in flight, "break" returns us to pinger. */
            if (in_flight () == 0)
                break;

            /* Otherwise, try to recvmsg() again. recvmsg()
             * is nonblocking after the first iteration, so that
             * if nothing is queued, it will receive EAGAIN
             * and return to pinger. */
        }
    }
    finish ();

    /*
       完成，打印出统计数据
    */
}

int gather_statistics (__u8 * ptr, int cc, __u16 seq, int hops, int csfailed, struct timeval *tv, char *from)
{
    int dupflag = 0;

    long triptime = 0;

    ++nreceived;
    if (!csfailed)
        acknowledge (seq);

    if (timing && cc >= 8 + sizeof (struct timeval))
    {
        struct timeval tmp_tv;

        memcpy (&tmp_tv, ptr, sizeof (tmp_tv));

restamp:
        tvsub (tv, &tmp_tv);

        /*
           得到接受时间和报文发送时间差
        */
        triptime = tv->tv_sec * 1000000 + tv->tv_usec;
        if (triptime < 0)
        {

            /*
               接受时间和报文发送的时间要小
               说明主机时间发生了回绕
            */
            fprintf (stderr, "Warning: time of day goes back (%ldus), taking countermeasures.\n", triptime);
            triptime = 0;
            if (!(options & F_LATENCY))
            {
                gettimeofday (tv, NULL);
                options |= F_LATENCY;
                goto restamp;
            }
        }
        if (!csfailed)
        {
            tsum += triptime;
            tsum2 += (long long) triptime *(long long) triptime;

            if (triptime < tmin)
                tmin = triptime;
            if (triptime > tmax)
                tmax = triptime;
            if (!rtt)
                rtt = triptime * 8;
            else
                rtt += triptime - rtt / 8;

            /*
               指数加权移动平均算法来估计下次的RTT值
            */
            if (options & F_ADAPTIVE)
                update_interval ();
        }
    }

    if (csfailed)
    {
        ++nchecksum;
        --nreceived;
    }
    else if (TST (seq % mx_dup_ck))
    {
        ++nrepeats;
        --nreceived;
        dupflag = 1;
    }
    else
    {
        SET (seq % mx_dup_ck);
        dupflag = 0;
    }
    confirm = confirm_flag;

    if (options & F_QUIET)
        return 1;
    if (options & F_FLOOD)
    {
        if (!csfailed)
            write (STDOUT_FILENO, "\b \b", 3);
        else
            write (STDOUT_FILENO, "\bC", 1);

        /*
           '\b'是backsapce
           注意只写一个字节，就是删除了一个点
           点的个数就代表了发送出去但是还没有接到回复的报文数
           比较奇怪的是下面的程序输出也不会显示出C，但是确实打印出了C，因为复制到文本里是可以看出C的
           printf("look here\bC\n");
        */

    }
    else
    {
        int i;

        __u8 *cp, *dp;

        printf ("%d bytes from %s: icmp_seq=%u", cc, from, seq);

        if (hops >= 0)
            printf (" ttl=%d", hops);

        if (cc < datalen + 8)
        {
            printf (" (truncated)\n");
            return 1;
        }
        if (timing)
        {
            if (triptime >= 100000)
                printf (" time=%ld ms", triptime / 1000);
            else if (triptime >= 10000)
                printf (" time=%ld.%01ld ms", triptime / 1000, (triptime % 1000) / 100);
            else if (triptime >= 1000)
                printf (" time=%ld.%02ld ms", triptime / 1000, (triptime % 1000) / 10);
            else
                printf (" time=%ld.%03ld ms", triptime / 1000, triptime % 1000);
        }
        if (dupflag)

            /*
               ping广播地址会出现这种情况
            */
            printf (" (DUP!)");
        if (csfailed)
            printf (" (BAD CHECKSUM!)");

        /* check the data */
        cp = ((u_char *) ptr) + sizeof (struct timeval);
        dp = &outpack[8 + sizeof (struct timeval)];
        for (i = sizeof (struct timeval); i < datalen; ++i, ++cp, ++dp)
        {
            if (*cp != *dp)
            {
                printf ("\nwrong data byte #%d should be 0x%x but was 0x%x", i, *dp, *cp);
                cp = (u_char *) ptr + sizeof (struct timeval);
                for (i = sizeof (struct timeval); i < datalen; ++i, ++cp)
                {
                    if ((i % 32) == sizeof (struct timeval))
                        printf ("\n#%d\t", i);
                    printf ("%x ", *cp);
                }
                break;
            }
        }
    }
    return 0;
}

static long llsqrt (long long a)
{
    long long prev = ~((long long) 1 << 63);

    long long x = a;

    if (x > 0)
    {
        while (x < prev)
        {
            prev = x;
            x = (x + (a / x)) / 2;
        }
    }

    return (long) x;
}

/*
 * finish --
 *    Print out statistics, and give up.
 */
void finish (void)
{

    /*此程序是在main_loop()函数将要退出的时候执行*/
    struct timeval tv = cur_time;

    tvsub (&tv, &start_time);

    putchar ('\n');
    fflush (stdout);
    printf ("--- %s ping statistics ---\n", hostname);
    printf ("%ld packets transmitted, ", ntransmitted);
    printf ("%ld received", nreceived);
    if (nrepeats)
        printf (", +%ld duplicates", nrepeats);
    if (nchecksum)
        printf (", +%ld corrupted", nchecksum);
    if (nerrors)
        printf (", +%ld errors", nerrors);
    if (ntransmitted)
    {
        printf (", %d%% packet loss", (int) ((((long long) (ntransmitted - nreceived)) * 100) / ntransmitted));
        printf (", time %ldms", 1000 * tv.tv_sec + tv.tv_usec / 1000);
    }
    putchar ('\n');

    if (nreceived && timing)
    {
        long tmdev;

        tsum /= nreceived + nrepeats;
        tsum2 /= nreceived + nrepeats;
        tmdev = llsqrt (tsum2 - tsum * tsum);

        printf ("rtt min/avg/max/mdev = %ld.%03ld/%lu.%03ld/%ld.%03ld/%ld.%03ld ms",
                (long) tmin / 1000, (long) tmin % 1000,
                (unsigned long) (tsum / 1000), (long) (tsum % 1000),
                (long) tmax / 1000, (long) tmax % 1000, (long) tmdev / 1000, (long) tmdev % 1000);
    }
    if (pipesize > 1)
        printf (", pipe %d", pipesize);
    if (ntransmitted > 1 && (!interval || (options & (F_FLOOD | F_ADAPTIVE))))
    {
        int ipg = (1000000 * (long long) tv.tv_sec + tv.tv_usec) / (ntransmitted - 1);

        printf (", ipg/ewma %d.%03d/%d.%03d ms", ipg / 1000, ipg % 1000, rtt / 8000, (rtt / 8) % 1000);
    }
    putchar ('\n');
    exit (!nreceived || (deadline && nreceived < npackets));
}


void status (void)
{
    int loss = 0;

    long tavg = 0;

    status_snapshot = 0;

    if (ntransmitted)
        loss = (((long long) (ntransmitted - nreceived)) * 100) / ntransmitted;

    fprintf (stderr, "\r%ld/%ld packets, %d%% loss", ntransmitted, nreceived, loss);

    if (nreceived && timing)
    {
        tavg = tsum / (nreceived + nrepeats);

        fprintf (stderr, ", min/avg/ewma/max = %ld.%03ld/%lu.%03ld/%d.%03d/%ld.%03ld ms",
                (long) tmin / 1000, (long) tmin % 1000,
                tavg / 1000, tavg % 1000, rtt / 8000, (rtt / 8) % 1000, (long) tmax / 1000, (long) tmax % 1000);

        /*
           ewma貌似是exponentially weighted moving average
           也就是指数加权移动平均...
           在通信领域中，EWMA主要用于对网络的状态参数进行估计和平滑，
           例如在TCP 拥塞控制中EWMA被 用来计算分组的往返时延( RTT )
        */
    }
    fprintf (stderr, "\n");
}
