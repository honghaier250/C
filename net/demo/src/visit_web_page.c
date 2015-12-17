/*************************************
  文件名： client.c
  linux 下socket网络编程简例  -
  客户端程序:访问网页
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char **argv)
{
    int fd;                        /* 文件描述符 */
    char str[32];
    int port = 80;
    int recbytes;
    int sin_size;
    char *msg = "GET / HTTP/1.1\r\nHost: baidu.com\r\n\r\n";
    char *ptr, **pptr;
    struct hostent *hptr;
    char buffer[1024] = { 0 };    /* 接受缓冲区 */
    struct sockaddr_in s_add, c_add;    /* 存储服务端和本端的ip、端口等信息结构体 */

    /* 建立socket 使用因特网，TCP流传输 */
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == fd)
    {
        printf("socket fail !\n");
        return -1;
    }

    /* 取得命令后第一个参数，即要解析的IP地址 */
    ptr = argv[1];

    /* 调用gethostbyname()。调用结果都存在hptr中 */
    if ((hptr = gethostbyname(ptr)) == NULL)
    {
        printf("gethostbyname error for host:%s\n", ptr);
        return 0;                /* 如果调用gethostbyname发生错误，返回1 */
    }

    pptr = hptr->h_addr_list;
    inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str));

    /* 构造服务器端的ip和端口信息，具体结构体可以查资料 */
    bzero(&s_add, sizeof(struct sockaddr_in));
    s_add.sin_family = AF_INET;
    s_add.sin_addr.s_addr = inet_addr(str);    /* ip转换为4字节整形，使用时需要根据服务端ip进行更改 */
    s_add.sin_port = htons(port);    /* 这里htons是将short型数据字节序由主机型转换为网络型，其实就是
                                       将2字节数据的前后两个字节倒换，和对应的ntohs效果、实质相同，只不过名字不同。htonl和ntohl是 操作的4字节整形。将0x12345678变为0x78563412，名字不同，内容两两相同，一般情况下网络为大端，
                                       PPC的cpu为大端，x86的cpu为小端，arm的可以配置大小端，需要保证接收时字节序正确。
                                     */
    printf("s_addr = %#x ,port : %#x\n", s_add.sin_addr.s_addr, s_add.sin_port);    /* 这里打印出的是小端 和我们平时看到的是相反的。 */

    /* 客户端连接服务器，参数依次为socket文件描述符，地址信息，地址结构大小 */
    if (-1 == connect(fd, (struct sockaddr *) (&s_add), sizeof(struct sockaddr)))
    {
        printf("connect fail !\n");
        return -1;
    }

    if (-1 == write(fd, msg, strlen(msg)))
    {
        printf("write fail !\n");
        return -1;
    }

    /*连接成功,从服务端接收字符 */
    if (-1 == (recbytes = read(fd, buffer, 1024)))
    {
        printf("read data fail !\n");
        return -1;
    }

    buffer[recbytes] = '\0';
    printf("%s\n", buffer);
    getchar();                    /* 此句为使程序暂停在此处，可以使用netstat查看当前的连接 */
    close(fd);                    /* 关闭连接，本次通信完成 */
    return 0;
}
