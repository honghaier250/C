
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

/*static char sccsid[] = "from: @(#)tftpsubs.c    5.6 (Berkeley) 2/28/91";*/

/* static char rcsid[] = "$Id: tftpsubs.c,v 1.2 1993/08/01 18:07:04 mycroft Exp $"; */
#endif                            /* not lint */

/* Simple minded read-ahead/write-behind subroutines for tftp user and
   server.  Written originally with multiple buffers in mind, but current
   implementation has two buffer logic wired in.

   Todo:  add some sort of final error check so when the write-buffer
   is finally flushed, the caller can detect if the disk filled up
   (or had an i/o error) and return a nak to the other side.

            Jim Guyton 10/85
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>

#include "tftp.h"

#define PKTSIZE SEGSIZE+4        /* should be moved to tftp.h */

struct bf
{
    int counter;                /* size of data in buffer, or flag */
    char buf[PKTSIZE];            /* room for data packet */
} bfs[2];

                /* Values for bf.counter  */
#define BF_ALLOC -3                /* alloc'd but not yet filled */
#define BF_FREE  -2                /* free */

/* [-1 .. SEGSIZE] = size of data in the data buffer */

static int nextone;                /* index of next buffer to use */

static int current;                /* index of buffer in use */

            /* control flags for crlf conversions */
int newline = 0;                /* fillbuf: in middle of newline expansion */

int prevchar = -1;                /* putbuf: previous char (cr check) */

struct tftphdr *rw_init (int);

struct tftphdr *w_init ()
{
    return rw_init (0);
}                                /* write-behind */

struct tftphdr *r_init ()
{
    return rw_init (1);
}                                /* read-ahead */

/* init for either read-ahead or write-behind */

/* x is zero for write-behind, one for read-head */
struct tftphdr *rw_init (int x)
{
    newline = 0;                /* init crlf flag */
    prevchar = -1;
    bfs[0].counter = BF_ALLOC;    /* pass out the first buffer */
    current = 0;
    bfs[1].counter = BF_FREE;
    nextone = x;                /* ahead or behind? */

/*
x为0，当为写时
x为1，当为读时
现在分析为什么初始的时候，nextone=1,current=0,会有先读的效果
而初始的时候，nextone=0,current=0,会有后写的效果。
在读的时候，nextone=1,current=0,会有先读的效果。

发送第一个数据的过程：
1. 将bfs[current]设置为没有读取数据。此时nextone=1,current=0。
2.current=!current。此时nextone=1,current=0变为nextone=1,current=1。
3. 再判断bfs[current]是不是没有存储数据如果没有数据则从文件中读出数据存入bfs[nextone]中。此时显然bfs[current]是没有保存数据的，应从文件里存储数据存入bfs[nextone]中读取数据。此时nextone=1,current=1。
4. 如果3执行，则nextone=!nextone。由于3执行了，所以此时nextone=1,current=1变为nextone=0,current=1。
5. 发送bfs[current]中的数据，此时nextone=0,current=1。
6. 将bfs[current]设置为没有读取数据。此时nextone=0,current=1。
7. ...

接收第一个数据的过程：
1. 将bfs[current]存储接受到的数据。此时nextone=0,current=0。
2. 然后current=!current。此时nextone=0,current=0变为nextone=0,current=1。
3. 判断bfs[current]是不是有数据，如果有数据则从bfs[nextone]中读出数据存入文件中。此时显然bfs[current]是没有接受数据的，故此不从bfs[nextone]中读取数据，此时nextone=0,current=1。
4. 如果3执行，则nextone=!nextone。由于3不执行，所以仍维持nextone=0,current=1。
5. 将bfs[current]存储接受到的数。此时nextone=0,current=1。
6. 然后current=!current。此时nextone=0,current=1变为nextone=0,current=0。
7. 判断bfs[current]是不是有数据，如果有数据则从bfs[nextone]中读出数据存入文件中。此时显然bfs[current]是有接受数据的，故此从bfs[nextone]中读取数据存入文件中，此时nextone=0,current=0。
8. 如果7执行则nextone=!nextone。由于7执行了所以此时nextone＝1,current=0。
9. ...

*/
    return (struct tftphdr *) bfs[0].buf;
}


/* Have emptied current buffer by sending to net and getting ack.
   Free it and return next buffer filled with data.
 */
int readit (FILE * file, struct tftphdr **dpp, int convert)
{
    struct bf *b;

    bfs[current].counter = BF_FREE;    /* free old one */
    current = !current;            /* "incr" current */

    b = &bfs[current];            /* look at new buffer */
    if (b->counter == BF_FREE)    /* if it's empty */
        read_ahead (file, convert);    /* fill it */
#if 0
    assert (b->counter != BF_FREE);    /* check */
#endif
    *dpp = (struct tftphdr *) b->buf;    /* set caller's ptr */
    return b->counter;
}

/*
 * fill the input buffer, doing ascii conversions if requested
 * conversions are  lf -> cr,lf  and cr -> cr, nul
 */
void read_ahead (FILE * file, int convert)
{
    register int i;

    register char *p;

    register int c;

    struct bf *b;

    struct tftphdr *dp;

    b = &bfs[nextone];            /* look at "next" buffer */

/*
bfs[]数组是有两个元素的数组
*/
    if (b->counter != BF_FREE)    /* nop if not free */
        return;
    nextone = !nextone;            /* "incr" next buffer ptr */

/*
指向数组的另一个元素
*/
    dp = (struct tftphdr *) b->buf;

    if (convert == 0)
    {

/*
octet模式，读取二进制数据
*/
        b->counter = read (fileno (file), dp->th_data, SEGSIZE);

/*
The function fileno() examines the argument stream and returns its integer descriptor. 
*/
        return;
    }

    p = dp->th_data;
    for (i = 0; i < SEGSIZE; i++)
    {
        if (newline)
        {

/*
上一个字符是标识新行的字符
*/
            if (prevchar == '\n')

/*
如果上一个字符是'\n'，则发送字符'\n'
*/
                c = '\n';        /* lf to cr,lf */
            else
                c = '\0';        /* cr to cr,nul */

/*
如果上一个字符不是'\n'，即上一个字符是'\r'，则发送字符'\0'
*/
            newline = 0;
        }
        else
        {
            c = getc (file);
            if (c == EOF)
                break;
            if (c == '\n' || c == '\r')
            {

/*
'\r'和'\n'都是标识新行的字符
*/
                prevchar = c;
                c = '\r';
                newline = 1;
            }
        }

/*
上面的程序保证'\r'则传送"\r\0"；'\n'则传送"\r\n"
*/
        *p++ = c;
    }
    b->counter = (int) (p - dp->th_data);
}

/* Update count associated with the buffer, get new buffer
   from the queue.  Calls write_behind only if next buffer not
   available.
 */
int writeit (FILE * file, struct tftphdr **dpp, int ct, int convert)
{
    bfs[current].counter = ct;    /* set size of data to write */

/*
将当前数组元素的counter为接受到报文的数据部分长度
*/
    current = !current;            /* switch to other buffer */
    if (bfs[current].counter != BF_FREE)    /* if not free */

/*
不是free的就需要更新
*/
        write_behind (file, convert);    /* flush it */
    bfs[current].counter = BF_ALLOC;    /* mark as alloc'd */
    *dpp = (struct tftphdr *) bfs[current].buf;
    return ct;                    /* this is a lie of course */
}

/*
 * Output a buffer to a file, converting from netascii if requested.
 * CR,NUL -> CR  and CR,LF => LF.
 * Note spec is undefined if we get CR as last byte of file or a
 * CR followed by anything else.  In this case we leave it alone.
 */
int write_behind (FILE * file, int convert)
{
    char *buf;

    int count;

    register int ct;

    register char *p;

    register int c;                /* current character */

    struct bf *b;

    struct tftphdr *dp;

    b = &bfs[nextone];

/*
取出下一个读取缓冲元素
*/
    if (b->counter < -1)        /* anything to flush? */

/*
该元素没有被更新，为BF_FREE或者BF_ALLOC
*/
        return 0;                /* just nop if nothing to do */

/*
元素里面保存了数据，b->counter为字节数
*/
    count = b->counter;            /* remember byte count */
    b->counter = BF_FREE;        /* reset flag */
    dp = (struct tftphdr *) b->buf;
    nextone = !nextone;            /* incr for next time */
    buf = dp->th_data;

    if (count <= 0)
        return -1;                /* nak logic? */

    if (convert == 0)

/*
octet模式，写二进制数据
*/
        return write (fileno (file), buf, count);

    p = buf;
    ct = count;
    while (ct--)
    {                            /* loop over the buffer */
        c = *p++;                /* pick up a character */
        if (prevchar == '\r')
        {                        /* if prev char was cr */
            if (c == '\n')        /* if have cr,lf then just */
                fseek (file, -1, 1);    /* smash lf on top of the cr */

/*
如果是'\n'字符，则倒退一个字符，覆盖'\r'
这保证了如果接受到"\r\n"，则保存字符'\n'
*/
            else if (c == '\0')    /* if have cr,nul then */

/*
如果是'\0'，则不保存字符
这保证了如果接受到"\r\0"，则保存字符'\r'
*/
                goto skipit;    /* just skip over the putc */
            /* else just fall through and allow it */
        }
        putc (c, file);
      skipit:
        prevchar = c;
    }
    return count;
}


/* When an error has occurred, it is possible that the two sides
 * are out of synch.  Ie: that what I think is the other side's
 * response to packet N is really their response to packet N-1.
 *
 * So, to try to prevent that, we flush all the input queued up
 * for us on the network connection on our host.
 *
 * We return the number of packets we flushed (mostly for reporting
 * when trace is active).
 */

int synchnet (int f)
{
    int j = 0;

    char dummy;

    while (1)
    {
        if (recv (f, &dummy, 1, MSG_DONTWAIT) < 0)

/*
非阻塞地接受所有的信息。
MSG_DONTWAIT:Enables non-blocking operation; if the operation would block, EAGAIN is returned.
*/
            break;
        j++;
    }
    return j;
}
