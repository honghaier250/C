#include <openssl/rand.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <errno.h>
#include <curses.h>

#define PORT 443
#define SERVER "127.0.0.1"
#define CACERT "./certs/rsa_ca_cert.pem"
#define MYCERTF "./certs/rsa_client_cert.pem"
#define MYKEYF "./certs/rsa_client_prikey.pem"
#define MSGLENGTH 1024
int main()
{
    struct sockaddr_in sin;

    int seed_int[100];

    SSL *ssl;

    SSL_METHOD *meth;

    SSL_CTX *ctx;

    int i, sock;

    //加载EVP cipher/digest算法
    OpenSSL_add_ssl_algorithms();

    //加载crypto/ssl错误提示
    SSL_load_error_strings();

    //为客户端构造TLSv1 SSL_METHOD结构
    meth = (SSL_METHOD *) TLSv1_client_method();

    //建立新的SSL上下文
    ctx = SSL_CTX_new(meth);
    if (NULL == ctx)
        exit(1);
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    //加载可信任证书库: 为SSL_CTX对象提供一个默认的信任证书
    SSL_CTX_load_verify_locations(ctx, CACERT, NULL);

    //加载客户端证书
    if (0 == SSL_CTX_use_certificate_file(ctx, MYCERTF, SSL_FILETYPE_PEM))
    {
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    //加载客户端私钥文件
    if (0 == SSL_CTX_use_PrivateKey_file(ctx, MYKEYF, SSL_FILETYPE_PEM))
    {
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    //验证私钥与证书是否一致
    if (!SSL_CTX_check_private_key(ctx))
    {
        printf("Private key does not match the certificate public key\n");
        exit(1);
    }

    srand((unsigned) time(NULL));
    for (i = 0; i < 100; i++)
        seed_int[i] = rand();
    RAND_seed(seed_int, sizeof(seed_int));

    //提交一份自己能够支持的加密方法的列表
    SSL_CTX_set_cipher_list(ctx, "RC4-MD5");

    //设置SSL_MODE_AUTO_RETRY标记
    SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);

    //首先建立连接
    printf("Begin tcp socket...\n");
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("SOCKET error. \n");
    }

    memset(&sin, '\0', sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(SERVER);    /* Server IP */
    sin.sin_port = htons(PORT);    /* Server Port number */

    int icnn = connect(sock, (struct sockaddr *) &sin, sizeof(sin));
    if (icnn == -1)
    {
        printf("can not connect to server,%s\n", strerror(errno));
        exit(1);
    }

    //安全连接要求在连接建立后进行握手
    ssl = SSL_new(ctx);
    if (NULL == ssl)
        exit(1);
    if (0 >= SSL_set_fd(ssl, sock))
    {
        printf("Attach to Line fail!\n");
        exit(1);
    }

    //请求SSL连接
    int k = SSL_connect(ssl);
    if (0 == k)
    {
        printf("%d\n", k);
        printf("SSL connect fail!\n");
        exit(1);
    }

    //进行安全会话
    printf("connect to server\n");
    char sendmsg[MSGLENGTH] = "\0";

    char revmsg[MSGLENGTH] = "\0";

    int err = SSL_read(ssl, revmsg, sizeof(revmsg));

    revmsg[err] = '\0';
    printf("%s\n", revmsg);
    while (1)
    {
        printf("please input the data to send:\n");
        scanf("%s", sendmsg);
        SSL_write(ssl, sendmsg, strlen(sendmsg));
        printf("send message ' %s ' success\n", sendmsg);
        if (strcmp(sendmsg, "quit") == 0)
        {
            break;
        }
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(sock);
    return 0;
}
