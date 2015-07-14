//server
#include <stdio.h>
#include <openssl/x509.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <curses.h>

#define MSGLENGTH 1024
#define PORT 443
#define CACERT "./demoCA/cacert.pem"
#define SVRCERTF "./certs/ec_servercert.pem"
#define SVRKEYF "./certs/ec_serverkey.key"
int main () 
{
    int sock;

    SSL_METHOD * meth;
    SSL_CTX * ctx;
    SSL * ssl;
    
        //加载EVP cipher/digest算法
        OpenSSL_add_ssl_algorithms ();
    
        //加载crypto/ssl错误提示
        SSL_load_error_strings ();
    
        //为服务器构造TLSv1 SSL_METHOD结构
        meth = (SSL_METHOD *) TLSv1_server_method ();
    
        //建立新的SSL上下文
        ctx = SSL_CTX_new (meth);
    if (NULL == ctx)
        exit (1);
    
        //设置证书认证回调函数
        SSL_CTX_set_verify (ctx, SSL_VERIFY_PEER, NULL);
    
        //加载可信任证书库: 为SSL_CTX对象提供一个默认的信任证书
        SSL_CTX_load_verify_locations (ctx, CACERT, NULL);
    
        //加载服务器端证书
        if (0 == SSL_CTX_use_certificate_file (ctx, SVRCERTF, SSL_FILETYPE_PEM))
        
    {
        ERR_print_errors_fp (stderr);
        exit (1);
    }
    
        //加载服务器私钥文件
        if (0 == SSL_CTX_use_PrivateKey_file (ctx, SVRKEYF, SSL_FILETYPE_PEM))
        
    {
        ERR_print_errors_fp (stderr);
        exit (1);
    }
    
        //验证私钥与证书是否一致
        if (!SSL_CTX_check_private_key (ctx))
        
    {
        printf ("Private key does not match the certificate public key\n");
        exit (1);
    }
    
        //提交一份自己能够支持的加密方法的列表
        SSL_CTX_set_cipher_list (ctx, "RC4-MD5");
    
        //设置SSL_MODE_AUTO_RETRY标记
        SSL_CTX_set_mode (ctx, SSL_MODE_AUTO_RETRY);
    
        //首先建立连接
        printf ("Begin tcp socket...\n");
    sock = socket (AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        
    {
        printf ("SOCKET error! \n");
        return 0;
    }
    struct sockaddr_in addr;

    memset (&addr, '\0', sizeof (addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons (PORT);    /* Server Port number */
    addr.sin_addr.s_addr = INADDR_ANY;
    int nResult = bind (sock, (struct sockaddr *) &addr, sizeof (addr));

    if (nResult == -1)
        
    {
        printf ("bind socket error\n");
        return 0;
    }
    printf ("server start successfully,port:%d\nwaiting for connections\n", PORT);
    struct sockaddr_in sa_cli;

    int err = listen (sock, 5);

    if (-1 == err)
        exit (1);
    int client_len = sizeof (sa_cli);

    int ss = accept (sock, (struct sockaddr *) &sa_cli, &client_len);

    if (ss == -1)
        
    {
        exit (1);
    }
    close (sock);
    printf ("Connection from %d, port %d\n", sa_cli.sin_addr.s_addr, sa_cli.sin_port);
    
        //安全连接要求在连接建立后进行握手
        ssl = SSL_new (ctx);
    if (NULL == ssl)
        exit (1);
    
        //将SSL与socket进行关联
        if (0 == SSL_set_fd (ssl, ss))
        
    {
        printf ("Attach to Line fail!\n");
        exit (1);
    }
    
        //接受SSL连接
    int k = SSL_accept (ssl);

    if (0 == k)
        
    {
        printf ("%d\n", k);
        printf ("SSL connect fail!\n");
        exit (1);
    }
    X509 * client_cert;
    
        //检索证书
        client_cert = SSL_get_peer_certificate (ssl);
    printf ("find a customer to try to connect\n");
    if (client_cert != NULL)
        
    {
        printf ("Client certificate:\n");
        
            //获得客户端证书subject并转变成字符型,以便进行打印
        char *str = X509_NAME_oneline (X509_get_subject_name (client_cert), 0, 0);

        if (NULL == str)
            
        {
            printf ("auth error!\n");
            exit (1);
        }
        printf ("subject: %s\n", str);
        
            //获得客户端证书issuer并转变成字符型,以便进行打印
            str = X509_NAME_oneline (X509_get_issuer_name (client_cert), 0, 0);
        if (NULL == str)
            
        {
            printf ("certificate name is null\n");
            exit (1);
        }
        printf ("issuer: %s\n", str);
        printf ("connect successfully\n");
        X509_free (client_cert);
        OPENSSL_free (str);
    }
    
    else
        
    {
        printf ("can not find the customer's certificate\n");
        exit (1);
    }
    
        //进行安全会话
    char buf[MSGLENGTH];

    SSL_write (ssl, "Server is connect to you!\n", strlen ("Server is connect to you!\n"));
    printf ("Listen to the client: \n");
    while (1)
        
    {
        err = SSL_read (ssl, buf, sizeof (buf));
        buf[err] = '\0';
        printf ("%s\n", buf);
        if (strcmp (buf, "quit") == 0)
            
        {
            break;
        }
    }
    SSL_shutdown (ssl);
    SSL_free (ssl);
    SSL_CTX_free (ctx);
    getch ();
    return 0;
}


