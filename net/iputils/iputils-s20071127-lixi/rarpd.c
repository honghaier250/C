
/*
 * rarpd.c    RARP daemon.
 *
 *        This program is free software; you can redistribute it and/or
 *        modify it under the terms of the GNU General Public License
 *        as published by the Free Software Foundation; either version
 *        2 of the License, or (at your option) any later version.
 *
 * Authors:    Alexey Kuznetsov, <kuznet@ms2.inr.ac.ru>
 */

#include <stdio.h>
#include <syslog.h>
#include <dirent.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <linux/if.h>
#include <linux/if_arp.h>
#include <netinet/in.h>
#include <linux/if_packet.h>
#include <linux/filter.h>

int do_reload = 1;

int debug;

int verbose;

int ifidx;

int allow_offlink;

int only_ethers;

int all_ifaces;

int listen_arp;

char *ifname;

char *tftp_dir = "/etc/tftpboot";

extern int ether_ntohost (char *name, unsigned char *ea);

void usage (void) __attribute__ ((noreturn));

#define lixiprintf printf
struct iflink
{
    struct iflink *next;
    int index;
    int hatype;
    unsigned char lladdr[16];
    char name[IFNAMSIZ];
    struct ifaddr *ifa_list;
} *ifl_list;

struct ifaddr
{
    struct ifaddr *next;
    __u32 prefix;
    __u32 mask;
    __u32 local;
};

struct rarp_map
{
    struct rarp_map *next;

    int ifindex;
    int arp_type;
    int lladdr_len;
    unsigned char lladdr[16];
    __u32 ipaddr;
} *rarp_db;

void usage ()
{
    fprintf (stderr, "Usage: rarpd [ -dveaA ] [ -b tftpdir ] [ interface]\n");
    exit (1);
}

void load_db (void)
{
}

void load_if (void)
{
    int fd;

    struct ifreq *ifrp, *ifend;

    struct iflink *ifl;

    struct ifaddr *ifa;

    struct ifconf ifc;

    struct ifreq ibuf[256];

    if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
    {

/*
取得一个UDP类型的socket描述符
SOCK_DGRAM表明：无连接的、不可靠的信息，信息的最大长度固定
*/

        syslog (LOG_ERR, "socket: %m");
        return;
    }

    ifc.ifc_len = sizeof ibuf;
    ifc.ifc_buf = (caddr_t) ibuf;

/*
在调用ioctl之前分配了缓冲区ibuf和一个ifconf结构ifc。
*/
    if (ioctl (fd, SIOCGIFCONF, (char *) &ifc) < 0 ||

/*
从内核获取系统配置中的所有接口，这是通过SIOCGIFCONF请求来实现的，它使用了ifconf结构，ifconf结构又使用了ifreq结构。
*/
        ifc.ifc_len < (int) sizeof (struct ifreq))
    {
        syslog (LOG_ERR, "SIOCGIFCONF: %m");
        close (fd);
        return;
    }

    while ((ifl = ifl_list) != NULL)
    {
        while ((ifa = ifl->ifa_list) != NULL)
        {
            ifl->ifa_list = ifa->next;
            free (ifa);
        }
        ifl_list = ifl->next;
        free (ifl);

/*
分配内存的时候，使用的是malloc函数，这里将分配的内存收回。
*/
    }

    ifend = (struct ifreq *) ((char *) ibuf + ifc.ifc_len);

/*
指向最后一个接口
*/
    for (ifrp = ibuf; ifrp < ifend; ifrp++)
    {

/*遍历所有接口*/
        __u32 addr;

        __u32 mask;

        __u32 prefix;

        if (ifrp->ifr_addr.sa_family != AF_INET)

/*
接口地址的种类不是IPv4，则进行下一个接口的判断
*/
            continue;
        addr = ((struct sockaddr_in *) &ifrp->ifr_addr)->sin_addr.s_addr;
        if (addr == 0)
            continue;
        if (ioctl (fd, SIOCGIFINDEX, ifrp))
        {

/*
Retrieve the interface index of the interface into ifr_ifindex. 
name -> if_index mapping
*/
            syslog (LOG_ERR, "ioctl(SIOCGIFNAME): %m");
            continue;
        }
        if (ifidx && ifrp->ifr_ifindex != ifidx)

/*
如果设置了接口标志（interface index），则要求监视的接口标志是设置的标志
*/
            continue;
        for (ifl = ifl_list; ifl; ifl = ifl->next)

/*
在列表中查找接口标志满足设置标志的列表项
*/
            if (ifl->index == ifrp->ifr_ifindex)
                break;
        if (ifl == NULL)
        {

/*
列表中没有满足要求的列表项
*/
            char *p;

            int index = ifrp->ifr_ifindex;

            if (ioctl (fd, SIOCGIFHWADDR, ifrp))
            {

/*
Get hardware address
*/
                syslog (LOG_ERR, "ioctl(SIOCGIFHWADDR): %m");
                continue;
            }

            ifl = (struct iflink *) malloc (sizeof (*ifl));

/*
为列表项分配内存
*/
            if (ifl == NULL)
                continue;
            memset (ifl, 0, sizeof (*ifl));
            ifl->next = ifl_list;
            ifl_list = ifl;

/*
将新建的列表项放在列表的最前面，列表指针指向最前面的列表项
*/
            ifl->index = index;
            ifl->hatype = ifrp->ifr_hwaddr.sa_family;
            memcpy (ifl->lladdr, ifrp->ifr_hwaddr.sa_data, 14);
            strncpy (ifl->name, ifrp->ifr_name, IFNAMSIZ);

/*
设置必要的的设置：
1. 下一个list
2. 接口标识
3. 地址种类
3. 地址
3. Interface request的名字，接口总是用名字来标识
*/
            p = strchr (ifl->name, ':');
            if (p)
                *p = 0;
            if (verbose)
                syslog (LOG_INFO, "link %s", ifl->name);
        }

/*
至此在列表中找到（或者新建）了符合要求的列表项
*/
        if (ioctl (fd, SIOCGIFNETMASK, ifrp))
        {

/*
get network PA mask
*/
            syslog (LOG_ERR, "ioctl(SIOCGIFMASK): %m");
            continue;
        }
        mask = ((struct sockaddr_in *) &ifrp->ifr_netmask)->sin_addr.s_addr;
        if (ioctl (fd, SIOCGIFDSTADDR, ifrp))
        {

/*
get remote PA address
在ifr_destaddr成员中返回点到点地址
*/
            syslog (LOG_ERR, "ioctl(SIOCGIFDSTADDR): %m");
            continue;
        }
        prefix = ((struct sockaddr_in *) &ifrp->ifr_dstaddr)->sin_addr.s_addr;
        for (ifa = ifl->ifa_list; ifa; ifa = ifa->next)
        {

/*
在链表中查找链表项目
*/
            if (ifa->local == addr && ifa->prefix == prefix && ifa->mask == mask)
                break;
        }
        if (ifa == NULL)
        {
            if (mask == 0 || prefix == 0)
                continue;
            ifa = (struct ifaddr *) malloc (sizeof (*ifa));
            memset (ifa, 0, sizeof (*ifa));
            ifa->local = addr;
            ifa->prefix = prefix;
            ifa->mask = mask;

/*
设置链表项的必要设置：
1. 地址
2. 前缀
3. 掩码
4. 下一个链表
*/

            ifa->next = ifl->ifa_list;
            ifl->ifa_list = ifa;

/*
将新链表项设置为链表的第一个链表项
*/
            if (verbose)
            {

/*
打印出冗余信息
*/
                int i;

                __u32 m = ~0U;

                for (i = 32; i >= 0; i--)
                {
                    if (htonl (m) == mask)
                        break;
                    m <<= 1;
                }
                if (addr == prefix)
                {
                    syslog (LOG_INFO, "  addr %s/%d on %s\n", inet_ntoa (*(struct in_addr *) &addr), i, ifl->name);
                }
                else
                {
                    char tmpa[64];

                    sprintf (tmpa, "%s", inet_ntoa (*(struct in_addr *) &addr));
                    syslog (LOG_INFO, "  addr %s %s/%d on %s\n", tmpa,
                            inet_ntoa (*(struct in_addr *) &prefix), i, ifl->name);
                }
            }
        }
    }
}

void configure (void)
{

/*
配置
*/
    load_if ();
    load_db ();
}

int bootable (__u32 addr)
{

/*
是不是bootable的
*/
    struct dirent *dent;

    DIR *d;

    char name[9];

    sprintf (name, "%08X", (__u32) ntohl (addr));
    d = opendir (tftp_dir);

/*
打开文件夹
文件夹按照文件夹流（directory stream）的方式组织起来
*/
    if (d == NULL)
    {
        syslog (LOG_ERR, "opendir: %m");
        return 0;
    }
    while ((dent = readdir (d)) != NULL)
    {

/*
一直读取文件夹流，直到文件夹的名字和目的名字相等为止
*/
        if (strncmp (dent->d_name, name, 8) == 0)
            break;
    }
    closedir (d);

/*
关闭文件夹流
*/
    return dent != NULL;
}

struct ifaddr *select_ipaddr (int ifindex, __u32 * sel_addr, __u32 ** alist)
{
    struct iflink *ifl;

    struct ifaddr *ifa;

    int retry = 0;

    int i;

  retry:
    for (ifl = ifl_list; ifl; ifl = ifl->next)

/*
遍历查找
*/
        if (ifl->index == ifindex)
            break;
    if (ifl == NULL && !retry)
    {
        retry++;
        load_if ();

/*
重新load
*/
        goto retry;
    }
    if (ifl == NULL)

/*
没找到，返回NULL
*/
        return NULL;

    for (i = 0; alist[i]; i++)
    {

/*
对于请求主机的每一个IPv4或者IPv6地址，查找列表项中是不是有匹配地址
*/
        __u32 addr = *(alist[i]);

        for (ifa = ifl->ifa_list; ifa; ifa = ifa->next)
        {
            if (!((ifa->prefix ^ addr) & ifa->mask))
            {

/*
一旦找到地址匹配项，就返回。
*/
                *sel_addr = addr;
                return ifa;
            }
        }
        if (ifa == NULL && retry == 0)
        {

/*
没找到，没有重试过，则重新load
*/
            retry++;
            load_if ();
            goto retry;
        }
    }
    if (i == 1 && allow_offlink)
    {
        *sel_addr = *(alist[0]);
        return ifl->ifa_list;
    }
    syslog (LOG_ERR, "Off-link request on %s", ifl->name);

/*
没有找到
*/
    return NULL;
}

struct rarp_map *rarp_lookup (int ifindex, int hatype, int halen, unsigned char *lladdr)
{

/*
在数据库中查找
*/
    struct rarp_map *r;

    for (r = rarp_db; r; r = r->next)
    {

/*
arp_type
lladdr_le
ifindex
设置了的就必须一致
*/
        if (r->arp_type != hatype && r->arp_type != -1)
            continue;
        if (r->lladdr_len != halen)
            continue;
        if (r->ifindex != ifindex && r->ifindex != 0)
            continue;
        if (memcmp (r->lladdr, lladdr, halen) == 0)

/*
将地址保存下来
*/
            break;
    }

    if (r == NULL)
    {

/*
没有找到对应的项
*/
        if (hatype == ARPHRD_ETHER && halen == 6)
        {

/*
ARPHRD_ETHER：值为1，表示是以太网地址
地址长度为6byte
*/
            struct ifaddr *ifa;

            struct hostent *hp;

            char ename[256];

            static struct rarp_map emap = {
                NULL,
                0,
                ARPHRD_ETHER,
                6,
            };

            if (ether_ntohost (ename, lladdr) != 0 || (hp = gethostbyname (ename)) == NULL)
            {

/*
gethostbyname：返回一个指向结构hostent的指针，该结构包含了该主机的所有IPv4地址或IPv6地址。在这些地址中就有我们需要的地址
*/
                if (verbose)
                    syslog (LOG_INFO, "not found in /etc/ethers");

/*
/etc/ethers contains 48 bit Ethernet addresses and their corresponding IP numbers, one line for each IP number.
*/
                return NULL;
            }
            if (hp->h_addrtype != AF_INET)
            {

/*
不是IP地址
*/
                syslog (LOG_ERR, "no IP address");
                return NULL;
            }
            ifa = select_ipaddr (ifindex, &emap.ipaddr, (__u32 **) hp->h_addr_list);

/*
选择对应接口的对应IP地址
h_addr_list：指向IPv4或者IPv6地址的指针数组
*/
            if (ifa)
            {
                memcpy (emap.lladdr, lladdr, 6);
                if (only_ethers || bootable (emap.ipaddr))
                    return &emap;
                if (verbose)
                    syslog (LOG_INFO, "not bootable");
            }
        }
    }
    return r;
}

static int load_arp_bpflet (int fd)
{
    static struct sock_filter insns[] = {
        BPF_STMT (BPF_LD | BPF_H | BPF_ABS, 6),
        BPF_JUMP (BPF_JMP | BPF_JEQ | BPF_K, ARPOP_RREQUEST, 0, 1),
        BPF_STMT (BPF_RET | BPF_K, 1024),
        BPF_STMT (BPF_RET | BPF_K, 0),
    };
    static struct sock_fprog filter = {
        sizeof insns / sizeof (insns[0]),
        insns
    };

    return setsockopt (fd, SOL_SOCKET, SO_ATTACH_FILTER, &filter, sizeof (filter));
}

int put_mylladdr (unsigned char **ptr_p, int ifindex, int alen)
{

/*
取得MAC地址
*/
    struct iflink *ifl;

    for (ifl = ifl_list; ifl; ifl = ifl->next)
        if (ifl->index == ifindex)
            break;

    if (ifl == NULL)
        return -1;

    memcpy (*ptr_p, ifl->lladdr, alen);
    *ptr_p += alen;
    return 0;
}

int put_myipaddr (unsigned char **ptr_p, int ifindex, __u32 hisipaddr)
{

/*
取得IP地址
*/
    __u32 laddr = 0;

    struct iflink *ifl;

    struct ifaddr *ifa;

    for (ifl = ifl_list; ifl; ifl = ifl->next)
        if (ifl->index == ifindex)
            break;

    if (ifl == NULL)
        return -1;

    for (ifa = ifl->ifa_list; ifa; ifa = ifa->next)
    {
        if (!((ifa->prefix ^ hisipaddr) & ifa->mask))
        {

/*
IP与前缀按位异或，再与掩码按位与
如果得出的是0，就匹配
*/
            laddr = ifa->local;
            break;
        }
    }
    memcpy (*ptr_p, &laddr, 4);
    *ptr_p += 4;
    return 0;
}

void arp_advise (int ifindex, unsigned char *lladdr, int lllen, __u32 ipaddr)
{
    int fd;

    struct arpreq req;

    struct sockaddr_in *sin;

    struct iflink *ifl;

    for (ifl = ifl_list; ifl; ifl = ifl->next)
        if (ifl->index == ifindex)
            break;

    if (ifl == NULL)
        return;

    fd = socket (AF_INET, SOCK_DGRAM, 0);
    memset (&req, 0, sizeof (req));
    req.arp_flags = ATF_COM;
    sin = (struct sockaddr_in *) &req.arp_pa;
    sin->sin_family = AF_INET;
    sin->sin_addr.s_addr = ipaddr;
    req.arp_ha.sa_family = ifl->hatype;
    memcpy (req.arp_ha.sa_data, lladdr, lllen);
    memcpy (req.arp_dev, ifl->name, IFNAMSIZ);

    if (ioctl (fd, SIOCSARP, &req))
        syslog (LOG_ERR, "SIOCSARP: %m");
    close (fd);
}

void serve_it (int fd)
{
    exit (0);
    unsigned char buf[1024];

    struct sockaddr_ll sll;

    socklen_t sll_len = sizeof (sll);

    struct arphdr *a = (struct arphdr *) buf;

    struct rarp_map *rmap;

    unsigned char *ptr;

    int n;

    n = recvfrom (fd, buf, sizeof (buf), MSG_DONTWAIT, (struct sockaddr *) &sll, &sll_len);

/*
接收报文
*/
    if (n < 0)
    {
        if (errno != EINTR && errno != EAGAIN)
            syslog (LOG_ERR, "recvfrom: %m");
        return;
    }

    /* Do not accept packets for other hosts and our own ones */
    if (sll.sll_pkttype != PACKET_BROADCAST && sll.sll_pkttype != PACKET_MULTICAST && sll.sll_pkttype != PACKET_HOST)
        return;

    if (ifidx && sll.sll_ifindex != ifidx)
        return;

    if (n < sizeof (*a))
    {
        syslog (LOG_ERR, "truncated arp packet; len=%d", n);
        return;
    }

    /* Accept only RARP requests */
    if (a->ar_op != htons (ARPOP_RREQUEST))

/*
RARP请求报文
*/
        return;

    if (verbose)
    {
        int i;

        char tmpbuf[16 * 3];

        char *ptr = tmpbuf;

        for (i = 0; i < sll.sll_halen; i++)
        {
            if (i)
            {
                sprintf (ptr, ":%02x", sll.sll_addr[i]);
                ptr++;
            }
            else
                sprintf (ptr, "%02x", sll.sll_addr[i]);
            ptr += 2;
        }
        syslog (LOG_INFO, "RARP request from %s on if%d", tmpbuf, sll.sll_ifindex);
    }

    /* Sanity checks */

    /* 1. IP only -> pln==4 */
    if (a->ar_pln != 4)
    {
        syslog (LOG_ERR, "interesting rarp_req plen=%d", a->ar_pln);
        return;
    }
    /* 2. ARP protocol must be IP */
    if (a->ar_pro != htons (ETH_P_IP))
    {
        syslog (LOG_ERR, "rarp protocol is not IP %04x", ntohs (a->ar_pro));
        return;
    }
    /* 3. ARP types must match */
    if (htons (sll.sll_hatype) != a->ar_hrd)
    {
        switch (sll.sll_hatype)
        {
            case ARPHRD_FDDI:
                if (a->ar_hrd == htons (ARPHRD_ETHER) || a->ar_hrd == htons (ARPHRD_IEEE802))
                    break;
            default:
                syslog (LOG_ERR, "rarp htype mismatch");
                return;
        }
    }
    /* 3. LL address lengths must be equal */
    if (a->ar_hln != sll.sll_halen)
    {
        syslog (LOG_ERR, "rarp hlen mismatch");
        return;
    }
    /* 4. Check packet length */
    if (sizeof (*a) + 2 * 4 + 2 * a->ar_hln > n)
    {
        syslog (LOG_ERR, "truncated rarp request; len=%d", n);
        return;
    }
    /* 5. Silly check: if this guy set different source
       addresses in MAC header and in ARP, he is insane
     */
    if (memcmp (sll.sll_addr, a + 1, sll.sll_halen))
    {
        syslog (LOG_ERR, "this guy set different his lladdrs in arp and header");
        return;
    }
    /* End of sanity checks */

    /* Lookup requested target in our database */
    rmap = rarp_lookup (sll.sll_ifindex, sll.sll_hatype, sll.sll_halen, (unsigned char *) (a + 1) + sll.sll_halen + 4);

/*
在数据库中查找
*/
    if (rmap == NULL)
        return;

    /* Prepare reply. It is almost ready, we only
       replace ARP packet type, put our lladdr and
       IP address to source fileds,
       and fill target IP address.
     */
    a->ar_op = htons (ARPOP_RREPLY);
    ptr = (unsigned char *) (a + 1);
    if (put_mylladdr (&ptr, sll.sll_ifindex, rmap->lladdr_len))

/*
放入MAC地址
*/
        return;
    if (put_myipaddr (&ptr, sll.sll_ifindex, rmap->ipaddr))

/*
放入IP地址
*/
        return;
    /* It is already filled */
    ptr += rmap->lladdr_len;
    memcpy (ptr, &rmap->ipaddr, 4);
    ptr += 4;

    /* Update our ARP cache. Probably, this guy
       will not able to make ARP (if it is broken)
     */
    arp_advise (sll.sll_ifindex, rmap->lladdr, rmap->lladdr_len, rmap->ipaddr);

    /* Sendto is blocking, but with 5sec timeout */
    alarm (5);

/*
5秒钟之后发生计时中断
*/
    sendto (fd, buf, ptr - buf, 0, (struct sockaddr *) &sll, sizeof (sll));
    alarm (0);

/*
无限时间之后发生计时中断
*/
}

void catch_signal (int sig, void (*handler) (int))
{
    struct sigaction sa;

    memset (&sa, 0, sizeof (sa));
    sa.sa_handler = handler;
#ifdef SA_INTERRUPT
    sa.sa_flags = SA_INTERRUPT;
#endif
    sigaction (sig, &sa, NULL);
}

void sig_alarm (int signo)
{

/*
计时器中断处理程序不做任何事情
*/
}

void sig_hup (int signo)
{
    do_reload = 1;

/*
SIGHUP中断
*/
}

int main (int argc, char **argv)
{
    lixiprintf ("hello\n");
    struct pollfd pset[2];

    int psize;

    int opt;


    opterr = 0;
    while ((opt = getopt (argc, argv, "aAb:dvoe")) != EOF)
    {
        switch (opt)
        {
            case 'a':
                ++all_ifaces;
                break;

            case 'A':
                ++listen_arp;
                break;

            case 'd':
                ++debug;
                break;

            case 'v':
                ++verbose;
                break;

            case 'o':
                ++allow_offlink;
                break;

            case 'e':
                ++only_ethers;
                break;

            case 'b':
                tftp_dir = optarg;
                break;

            default:
                usage ();
        }
    }
    if (argc > optind)
    {
        if (argc > optind + 1)
            usage ();
        ifname = argv[optind];
    }

    psize = 1;
    pset[0].fd = socket (PF_PACKET, SOCK_DGRAM, 0);

    if (ifname)
    {
        struct ifreq ifr;

        memset (&ifr, 0, sizeof (ifr));
        strncpy (ifr.ifr_name, ifname, IFNAMSIZ);
        if (ioctl (pset[0].fd, SIOCGIFINDEX, &ifr))
        {
            perror ("ioctl(SIOCGIFINDEX)");
            usage ();
        }
        ifidx = ifr.ifr_ifindex;
    }
    lixiprintf ("ifname:%s\n", ifname);
    pset[1].fd = -1;
    if (listen_arp)
    {
        pset[1].fd = socket (PF_PACKET, SOCK_DGRAM, 0);
        if (pset[1].fd >= 0)
        {
            load_arp_bpflet (pset[1].fd);
            psize = 1;
        }
    }

    if (pset[1].fd >= 0)
    {
        struct sockaddr_ll sll;

        memset (&sll, 0, sizeof (sll));
        sll.sll_family = AF_PACKET;
        sll.sll_protocol = htons (ETH_P_ARP);
        sll.sll_ifindex = all_ifaces ? 0 : ifidx;
        if (bind (pset[1].fd, (struct sockaddr *) &sll, sizeof (sll)) < 0)
        {
            close (pset[1].fd);
            pset[1].fd = -1;
            psize = 1;
        }
    }
    if (pset[0].fd >= 0)
    {
        struct sockaddr_ll sll;

        memset (&sll, 0, sizeof (sll));
        sll.sll_family = AF_PACKET;
        sll.sll_protocol = htons (ETH_P_RARP);
        sll.sll_ifindex = all_ifaces ? 0 : ifidx;
        if (bind (pset[0].fd, (struct sockaddr *) &sll, sizeof (sll)) < 0)
        {
            close (pset[0].fd);
            pset[0].fd = -1;
        }
    }
    if (pset[0].fd < 0)
    {
        pset[0] = pset[1];
        psize--;
    }
    if (psize == 0)
    {
        fprintf (stderr, "failed to bind any socket. Aborting.\n");
        exit (1);
    }
    lixiprintf ("before fork\n");
    if (!debug)
    {
        int fd;

        pid_t pid = fork ();

/*
On success, the PID of the child process is returned in the parent's thread of execution, and a 0 is returned in the child's thread of execution. On failure, a -1 will be returned in the parent's context, no child process will be created, and errno will be set appropriately.
*/
        if (pid > 0)
        {

/*
父亲进程
*/
            lixiprintf ("Hello, I am the parent.\n");
            exit (0);
        }
        else if (pid == -1)
        {
            perror ("rarpd: fork");
            exit (1);
        }
        lixiprintf ("Hello, I am the child.\n");

/*
父进程和子进程共用资源，包括作为输出的终端，所以输出会打在一块儿；而且由于是在两个进程中执行，所以父进程和子进程说的话可能不是按照程序中排列的顺序。
父进程到这里已经结束了，下面的程序都在子进程里执行
*/
        chdir ("/");
        fd = open ("/dev/null", O_RDWR);

/*
/dev/null他是空设备，也称为位桶（bit bucket）。所有写入他的输出都会被抛弃。如果不想让消息以标准输出显示或写入文件，那么能将消息重定向到位桶。
*/
        lixiprintf ("The moment just before dup2()\n");
        if (fd >= 0)
        {
            dup2 (fd, 0);
            dup2 (fd, 1);
            dup2 (fd, 2);

/*
dup函数将标准输入、输出和错误输出重定向到/dev/null设备上
STDIN_FILENO:0
STDOUT_FILENO:1
STDERR_FILENO:2
int dup2(int oldfd, int newfd):dup2() makes newfd be the copy of oldfd, closing newfd first if necessary. 
*/
            if (fd > 2)
                close (fd);
        }

        setsid ();

/*
setsid函数将创建新的会话，并使得调用setsid函数的进程成为新会话的领头进程。调用setsid函数的进程是新创建会话中的惟一的进程组，进程组ID为调用进程的进程号。setsid函数产生这一结果还有个条件，即调用进程不为一个进程的领头进程。由于在第一步中调用fork的父进程退出，使得子进程不可能是进程组的领头进程。该会话的领头进程没有控制终端与其相连。至此，满足了守护进程没有控制终端的要求。
setsid() creates a new session if the calling process is not a process group leader. The calling process is the leader of the new session, the process group leader of the new process group, and has no controlling tty. The process group ID and session ID of the calling process are set to the PID of the calling process. The calling process will be the only process in this new process group and in this new session. 
*/
    }

    openlog ("rarpd", LOG_PID | LOG_CONS, LOG_DAEMON);

/*
openlog() opens a connection to the system logger for a program. 
*/
    catch_signal (SIGALRM, sig_alarm);
    catch_signal (SIGHUP, sig_hup);

/*
SIGHUP在守护进程中，通常用来重读配置文件
*/
    for (;;)
    {
        int i;

        if (do_reload)
        {
            configure ();
            do_reload = 0;
        }

#define EVENTS (POLLIN|POLLPRI|POLLERR|POLLHUP)
        pset[0].events = EVENTS;
        pset[0].revents = 0;
        pset[1].events = EVENTS;
        pset[1].revents = 0;

/*
POLLIN:There is data to read. 
POLLPRI:There is urgent data to read (e.g., out-of-band data on TCP socket; pseudo-terminal master in packet mode has seen state change in slave).
POLLERR:Error condition (output only). 
POLLHUP:Hang up (output only). 
*/
        i = poll (pset, psize, -1);

/*
poll(): it waits for one of a set of file descriptors to become ready to perform I/O. 
On success, a positive number is returned; this is the number of structures which have non-zero revents fields (in other words, those descriptors with events or errors reported). A value of 0 indicates that the call timed out and no file descriptors were ready. On error, -1 is returned, and errno is set appropriately. 
*/
        if (i <= 0)
        {

/*
出错或者超时
*/
            if (errno != EINTR && i < 0)
            {
                syslog (LOG_ERR, "poll returned some crap: %m\n");
                sleep (10);
            }
            continue;
        }
        for (i = 0; i < psize; i++)
        {

/*
服务之
*/
            if (pset[i].revents & EVENTS)
                serve_it (pset[i].fd);
        }
    }
}
