
/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
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
char copyright[] = "@(#) Copyright (c) 1983 Regents of the University of California.\n\
 All rights reserved.\n";
#endif                            /* not lint */

#ifndef lint

/*static char sccsid[] = "from: @(#)tftpd.c    5.13 (Berkeley) 2/26/91";*/

/*static char rcsid[] = "$Id: tftpd.c,v 1.3 1993/08/01 18:28:53 mycroft Exp $";*/
#endif                            /* not lint */

/*
 * Trivial file transfer protocol server.
 *
 * This version includes many modifications by Jim Guyton <guyton@rand-unix>
 */

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <setjmp.h>
#include <syslog.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "tftp.h"

#ifndef MSG_CONFIRM
#define MSG_CONFIRM 0
#warning Please, upgrade kernel, otherwise this tftpd has no advantages.
#endif

#define    TIMEOUT        5

int peer;

int rexmtval = TIMEOUT;

int maxtimeout = 5 * TIMEOUT;

#define    PKTSIZE    SEGSIZE+4
char buf[PKTSIZE];

char ackbuf[PKTSIZE];

union
{
    struct sockaddr sa;
    struct sockaddr_in sin;
    struct sockaddr_in6 sin6;
} from;

socklen_t fromlen;

#define MAXARG    1
char *dirs[MAXARG + 1];

void tftp (struct tftphdr *tp, int size) __attribute__ ((noreturn));

void nak (int error);

int validate_access (char *filename, int mode);

struct formats;

void sendfile (struct formats *pf);

void recvfile (struct formats *pf);


#define lixiprintf printf
int main (int ac, char **av)
{
    register struct tftphdr *tp;

    register int n = 0;

    int on = 1;

    /* Sanity. If parent forgot to setuid() on us. */
    if (geteuid () == 0)
    {
        setgid (65534);
        setuid (65534);

/*
effective就像用户分层一样，real是实际的用户，系统通过你属于哪一层的用户来决定你的访问权限
只有在超级用户调用时，有效用户ID才是0
*/ lixiprintf ("geteuid == 0\n");
    }

    ac--;
    av++;
    while (ac-- > 0 && n < MAXARG)
        dirs[n++] = *av++;

/*
确定tftpboot文件夹地址
*/
    openlog ("tftpd", LOG_PID, LOG_DAEMON);
    if (ioctl (0, FIONBIO, &on) < 0)
    {
        syslog (LOG_ERR, "ioctl(FIONBIO): %m\n");

/*
保存至系统记录
*/
        exit (1);
    }

    fromlen = sizeof (from);
    n = recvfrom (0, buf, sizeof (buf), 0, (struct sockaddr *) &from, &fromlen);

/*
socketfd是0，这是为什么
*/
    if (n < 0)
    {
        if (errno != EAGAIN)
            syslog (LOG_ERR, "recvfrom: %m\n");
        exit (1);
    }
    /*
     * Now that we have read the message out of the UDP
     * socket, we fork and exit.  Thus, inetd will go back
     * to listening to the tftp port, and the next request
     * to come in will start up a new instance of tftpd.
     *
     * We do this so that inetd can run tftpd in "wait" mode.
     * The problem with tftpd running in "nowait" mode is that
     * inetd may get one or more successful "selects" on the
     * tftp port before we do our receive, so more than one
     * instance of tftpd may be started up.  Worse, if tftpd
     * break before doing the above "recvfrom", inetd would
     * spawn endless instances, clogging the system.
     */
    {
        int pid;

        int i;

        socklen_t j;

        for (i = 1; i < 20; i++)
        {

/*
尝试创建一个进程20次
*/
            pid = fork ();
            if (pid < 0)
            {

/*
进程创建不成功
*/
                sleep (i);

/*
睡眠一段时间
*/
                /*
                 * flush out to most recently sent request.
                 *
                 * This may drop some request, but those
                 * will be resent by the clients when
                 * they timeout.  The positive effect of
                 * this flush is to (try to) prevent more
                 * than one tftpd being started up to service
                 * a single request from a single client.
                 */
                j = sizeof from;
                i = recvfrom (0, buf, sizeof (buf), 0, (struct sockaddr *) &from, &j);

/*
取得最近接受到的请求
*/
                if (i > 0)
                {
                    n = i;
                    fromlen = j;
                }
            }
            else
            {

/*
进程创建成功
*/
                break;
            }
        }

        if (pid < 0)
        {

/*
20次都没有成功创建一个进程
*/
            syslog (LOG_ERR, "fork: %m\n");
            exit (1);
        }
        else if (pid != 0)
        {

/*
结束父进程
*/
            exit (0);
        }
    }

/*
下面运行的都是子进程
*/
    alarm (0);
    close (0);
    close (1);

/*
关闭标准输入输出终端
*/
    peer = socket (from.sa.sa_family, SOCK_DGRAM, 0);

/*
取得一个和请求一样类型的套接字
*/
    if (peer < 0)
    {
        syslog (LOG_ERR, "socket: %m\n");
        exit (1);
    }
    if (connect (peer, (struct sockaddr *) &from, sizeof (from)) < 0)
    {

/*
连接套接字
*/
        syslog (LOG_ERR, "connect: %m\n");
        exit (1);
    }
    tp = (struct tftphdr *) buf;
    tp->th_opcode = ntohs (tp->th_opcode);
    if (tp->th_opcode == RRQ || tp->th_opcode == WRQ)

/*
如果请求是读请求和写请求
*/
        tftp (tp, n);
    exit (1);
}

struct formats
{
    char *f_mode;
    int (*f_validate) (char *filename, int mode);
    void (*f_send) (struct formats *);
    void (*f_recv) (struct formats *);
    int f_convert;
} formats[] =
{
    {
    "netascii", validate_access, sendfile, recvfile, 1},

/*
8位的ASCII码形式，f_convert为0表示读取ASCII字符
*/
    {
    "octet", validate_access, sendfile, recvfile, 0},

/*
8位源数据类型，f_convert为0表示直接读取二进制数据
*/
#ifdef notdef
    {
    "mail", validate_user, sendmail, recvmail, 1},

/*
mail类型已经不再支持
*/
#endif
    {
    0}
};

/*
 * Handle initial connection protocol.
 */
void tftp (struct tftphdr *tp, int size)
{

/*
处理初始化连接协议
*/
    register char *cp;

    int first = 1, ecode;

    register struct formats *pf;

    char *filename, *mode = NULL;

    filename = cp = tp->th_stuff;

/*
文件名
*/
  again:
    while (cp < buf + size)
    {

/*
在报文里查找文件名或者模式的'\0'
*/
        if (*cp == '\0')
            break;
        cp++;
    }
    if (*cp != '\0')
    {

/*
没有找到结束字符'\0'
*/
        nak (EBADOP);
        exit (1);
    }
    if (first)
    {

/*
第一次读取文件名字段，第二次读取模式字段
*/
        mode = ++cp;

/*
模式字段紧接着文件名字段的结束字符'\0'
*/
        first = 0;
        goto again;
    }
    for (cp = mode; *cp; cp++)
        if (isupper (*cp))
            *cp = tolower (*cp);

/*
将模式字符全部转为小写
*/
    for (pf = formats; pf->f_mode; pf++)
        if (strcmp (pf->f_mode, mode) == 0)
            break;

/*
formats[]数组中定义了很多种模式的具体处理办法，如果报文的模式字段和这里面某种模式匹配，则使用相应方法处理
*/

    if (pf->f_mode == 0)
    {

/*
formats中没有匹配的模式    
*/
        nak (EBADOP);
        exit (1);
    }
    ecode = (*pf->f_validate) (filename, tp->th_opcode);

/*
使用对应函数处理读请求和写请求
*/
    if (ecode)
    {

/*
处理出错
*/
        nak (ecode);
        exit (1);
    }
    if (tp->th_opcode == WRQ)
        (*pf->f_recv) (pf);

/*
如果接受到写请求则调用对应的接受函数
*/
    else
        (*pf->f_send) (pf);

/*
如果接受到写请求则调用对应的发送函数
*/
    exit (0);
}


FILE *file;

/*
 * Validate file access.  Since we
 * have no uid or gid, for now require
 * file to exist and be publicly
 * readable/writable.
 * If we were invoked with arguments
 * from inetd then the file must also be
 * in one of the given directory prefixes.
 * Note also, full path name must be
 * given as we have no login directory.
 */
int validate_access (char *filename, int mode)
{

/*
确认文件访问
*/
    struct stat stbuf;

    int fd;

    char *cp;

    char fnamebuf[1024 + 512];

    for (cp = filename; *cp; cp++)
    {
        if (*cp == '.' && (cp == filename || strncmp (cp - 1, "/../", 4) == 0))
        {

/*
tftpboot/.xxxx
tftpboot/xxxx/../
都出错
前者是隐藏文件，后者尝试访问上层目录，很有可能超出tftpboot文件夹
*/
            syslog (LOG_ERR, "bad path %s", filename);
            return (EACCESS);
        }
    }

    if (*filename == '/')
        filename++;

    if (!*dirs)
    {

/*
没有设置文件夹
*/
        syslog (LOG_ERR, "no dirs");
        return EACCESS;
    }
    snprintf (fnamebuf, sizeof (fnamebuf) - 1, "%s/%s", *dirs, filename);
    filename = fnamebuf;

/*
将文件绝对路径复制到fnamebuf中，filename指向这个路径
*/
    if (stat (filename, &stbuf) < 0)
    {

/*
stat() stats the file pointed to by path and fills in buf. 
On success, zero is returned. On error, -1 is returned, and errno is set appropriately. 
确定路径是否能访问，例如路径是不是存在，路径如果存在那么是不是有执行的权限（也就是查找的权限）。
*/
        syslog (LOG_ERR, "stat %s : %m", filename);
        return (errno == ENOENT ? ENOTFOUND : EACCESS);

/*
ENOENT：A component of the path path does not exist, or the path is an empty string. 
*/
    }
    if (mode == RRQ)
    {

/*
客户发送的是读请求
*/
        if ((stbuf.st_mode & (S_IREAD >> 6)) == 0)
        {

/*
S_IRUSR 或S_IREAD，256，代表该文件所有者具有可读取的权限。
S_IWUSR 或S_IWRITE，128，代表该文件所有者具有可写入的权限。
S_IXUSR 或S_IEXEC，64，代表该文件所有者具有可执行的权限。
*/
            syslog (LOG_ERR, "not readable %s", filename);
            return (EACCESS);
        }
    }
    else
    {
        if ((stbuf.st_mode & (S_IWRITE >> 6)) == 0)
        {
            syslog (LOG_ERR, "not writable %s", filename);
            return (EACCESS);
        }
    }
    fd = open (filename, mode == RRQ ? 0 : 1);

/*
O_RDONLY，0，以只读方式打开文件
O_WRONLY，1，以只写方式打开文件
*/
    if (fd < 0)
    {
        syslog (LOG_ERR, "cannot open %s: %m", filename);
        return (errno + 100);

/*
返回的错误加上100
*/
    }
    file = fdopen (fd, (mode == RRQ) ? "r" : "w");

/*
"r":Open text file for reading. The stream is positioned at the beginning of the file. 
"w":Truncate file to zero length or create text file for writing. The stream is positioned at the beginning of the file. 
*/
    if (file == NULL)
    {
        return errno + 100;

/*
返回的错误加上100
*/
    }
    return (0);

/*
没有出错返回0
*/
}

int confirmed;

int timeout;

jmp_buf timeoutbuf;

void timer (int signo)
{

/*
中断处理程序
*/
    confirmed = 0;
    timeout += rexmtval;
    if (timeout >= maxtimeout)
        exit (1);
    longjmp (timeoutbuf, 1);
}

/*
 * Send the requested file.
 */
void sendfile (struct formats *pf)
{
    struct tftphdr *dp;

    register struct tftphdr *ap;    /* ack packet */

    volatile int block = 1;

    int size, n;

    confirmed = 0;
    signal (SIGALRM, timer);

/*
安装中断处理程序
*/
    dp = r_init ();

/*
初始化存储缓冲
*/
    ap = (struct tftphdr *) ackbuf;
    do
    {
        size = readit (file, &dp, pf->f_convert);

/*

*/
        if (size < 0)
        {

/*
读文件出错
*/
            nak (errno + 100);
            goto abort;
        }
        dp->th_opcode = htons ((u_short) DATA);
        dp->th_block = htons ((u_short) block);
        timeout = 0;
        (void) setjmp (timeoutbuf);

      send_data:
        if (send (peer, dp, size + 4, confirmed) != size + 4)
        {

/*
发送文件
confirmed为0或者为MSG_CONFIRM
MSG_CONFIRM (Linux 2.3+ only)：Tell the link layer that forward progress happened: you got a successful reply from the other side.
*/
            syslog (LOG_ERR, "tftpd: write: %m\n");
            goto abort;
        }
        confirmed = 0;
        read_ahead (file, pf->f_convert);

/*
提前读取文件
*/
        for (;;)
        {

/*
一直循环，直到接到了对应报文的ACK
或者出错结束服务
或者超时重发
或者接到上一个报文的ACK，重发
*/
            alarm (rexmtval);    /* read the ack */
            n = recv (peer, ackbuf, sizeof (ackbuf), 0);

/*
在rexmtval秒的时间之内完成接受ACK。
如果没有完成，则中断处理程序里调用longjmp()函数，使得程序跳到setjmp()函数最后一次执行处执行，即跳到send_data标识处执行
*/
            alarm (0);
            if (n < 0)
            {
                syslog (LOG_ERR, "tftpd: read: %m\n");
                goto abort;
            }
            ap->th_opcode = ntohs ((u_short) ap->th_opcode);
            ap->th_block = ntohs ((u_short) ap->th_block);

            if (ap->th_opcode == ERROR)

/*
接受到报文的操作码是错误
*/
                goto abort;

            if (ap->th_opcode == ACK)
            {

/*
接受到报文的操作码是ACK
*/
                if (ap->th_block == block)
                {

/*
块编号为当前发送的块编号，则说明接到了对应发送块的ACK
confirmed设置为MSG_CONFIRM，表明收发已有所进展
*/
                    confirmed = MSG_CONFIRM;
                    break;
                }
                /* Re-synchronize with the other side */
                synchnet (peer);

/*
将所有的ACK报文清除
*/
                if (ap->th_block == (block - 1))
                {

/*
接受到了上一个TFTP报文的ACK，重发报文
*/
                    goto send_data;
                }
            }

        }
        block++;
    }
    while (size == SEGSIZE);

/*
这里是提前读，也就是发送之前提前读取了一个缓冲（可能SEGSIZE字节，也可能不足SEGSIZE字节）的数据
如果读取的字节数目不足SEGSIZE字节，则说明已到文件尾（EOF）
*/
  abort:
    (void) fclose (file);
}

void justquit (int signo)
{
    exit (0);
}


/*
 * Receive a file.
 */
void recvfile (struct formats *pf)
{

/*
和sendfile如此相像
*/
    struct tftphdr *dp;

    register struct tftphdr *ap;    /* ack buffer */

    volatile int block = 0, n, size;

    confirmed = 0;
    signal (SIGALRM, timer);
    dp = w_init ();
    ap = (struct tftphdr *) ackbuf;
    do
    {
        timeout = 0;
        ap->th_opcode = htons ((u_short) ACK);
        ap->th_block = htons ((u_short) block);
        block++;
        (void) setjmp (timeoutbuf);
      send_ack:
        if (send (peer, ackbuf, 4, confirmed) != 4)
        {

/*
confirmed为0或者为MSG_CONFIRM
MSG_CONFIRM (Linux 2.3+ only)：Tell the link layer that forward progress happened: you got a successful reply from the other side.
*/
            syslog (LOG_ERR, "tftpd: write: %m\n");
            goto abort;
        }
        confirmed = 0;
        write_behind (file, pf->f_convert);

/*
写
*/
        for (;;)
        {
            alarm (rexmtval);
            n = recv (peer, dp, PKTSIZE, 0);

/*
在rexmtval秒的时间之内完成接受报文。
如果没有完成，则中断处理程序里调用longjmp()函数，使得程序跳到setjmp()函数最后一次执行处执行，即跳到send_ack标识处执行
*/
            alarm (0);
            if (n < 0)
            {                    /* really? */
                syslog (LOG_ERR, "tftpd: read: %m\n");
                goto abort;
            }
            dp->th_opcode = ntohs ((u_short) dp->th_opcode);
            dp->th_block = ntohs ((u_short) dp->th_block);
            if (dp->th_opcode == ERROR)
                goto abort;

/*
接受到报文的操作码是错误
*/
            if (dp->th_opcode == DATA)
            {

/*
接受到报文的操作码是数据
*/
                if (dp->th_block == block)
                {

/*
接受到报文的块编号是当前准备接受的块编号
*/
                    confirmed = MSG_CONFIRM;
                    break;        /* normal */
                }
                /* Re-synchronize with the other side */
                (void) synchnet (peer);

/*
将所有的数据报文清除
*/
                if (dp->th_block == (block - 1))

/*
接到上一个数据报文，则重发当前报文的ACK
*/
                    goto send_ack;    /* rexmit */
            }
        }
        /*  size = write(file, dp->th_data, n - 4); */
        size = writeit (file, &dp, n - 4, pf->f_convert);

/*
n为TFTP数据报文长度，对于数据类型的报文，操作码为2字节，块编号为2字节，所以数据长度为n-4字节
*/
        if (size != (n - 4))
        {                        /* ahem */
            if (size < 0)
                nak (errno + 100);
            else
                nak (ENOSPACE);
            goto abort;
        }
    }
    while (size == SEGSIZE);
    write_behind (file, pf->f_convert);

/*
当接受到数据不足SEGSIZE（512byte）则将滞后写的数据写入文件中
*/
    (void) fclose (file);        /* close data file */

    ap->th_opcode = htons ((u_short) ACK);    /* send the "final" ack */
    ap->th_block = htons ((u_short) (block));
    (void) send (peer, ackbuf, 4, confirmed);

    signal (SIGALRM, justquit);    /* just quit on timeout */

/*
如果超时，马上退出程序
*/
    alarm (rexmtval);
    n = recv (peer, buf, sizeof (buf), 0);    /* normally times out and quits */
    alarm (0);
    if (n >= 4 &&                /* if read some data */
        dp->th_opcode == DATA &&    /* and got a data block */
        block == dp->th_block)
    {                            /* then my last ack was lost */

/*
接到了最后一个块的多余的数据
*/
        (void) send (peer, ackbuf, 4, 0);    /* resend final ack */
    }
  abort:
    return;
}

struct errmsg
{
    int e_code;
    char *e_msg;
} errmsgs[] =
{
    {
    EUNDEF, "Undefined error code"},
    {
    ENOTFOUND, "File not found"},
    {
    EACCESS, "Access violation"},
    {
    ENOSPACE, "Disk full or allocation exceeded"},
    {
    EBADOP, "Illegal TFTP operation"},
    {
    EBADID, "Unknown transfer ID"},
    {
    EEXISTS, "File already exists"},
    {
    ENOUSER, "No such user"},
    {
    -1, 0}
};

/*
 * Send a nak packet (error message).
 * Error code passed in is one of the
 * standard TFTP codes, or a UNIX errno
 * offset by 100.
 */
void nak (int error)
{

/*
发送一个错误报文
*/
    register struct tftphdr *tp;

    int length;

    register struct errmsg *pe;

    tp = (struct tftphdr *) buf;
    tp->th_opcode = htons ((u_short) ERROR);

/*
操作码为ERROR ＝ 5
*/
    tp->th_code = htons ((u_short) error);

/*
差错码
*/
    for (pe = errmsgs; pe->e_code >= 0; pe++)
        if (pe->e_code == error)
            break;

/*
找到差错码对应的差错信息
*/
    if (pe->e_code < 0)
    {

/*
差错信息errmsgs[]数组最后一个的错误码为-1
这些错误都是由于open和fdopen函数发生错误。
*/
        pe->e_msg = strerror (error - 100);

/*
将差错信息设置为错误对应的字符串描述
The strerror() function returns a string describing the error code passed in the argument errnum.
*/
        tp->th_code = EUNDEF;    /* set 'undef' errorcode */

/*
错误码设置为未定义错误
*/
    }
    strcpy (tp->th_msg, pe->e_msg);

/*
差错报文字段是一个ASCII表示的字符串
*/
    length = strlen (pe->e_msg);
    tp->th_msg[length] = '\0';

/*
差错报文字段以'\0'结尾
*/
    length += 5;

/*
操作码2字节，差错码2字节，'\0'为1字节，所以需要再加上5字节长度
*/
    if (send (peer, buf, length, 0) != length)
        syslog (LOG_ERR, "nak: %m\n");
}
