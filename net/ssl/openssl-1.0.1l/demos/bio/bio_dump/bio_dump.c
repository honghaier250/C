#include <stdio.h>
#include <openssl/bio.h>
#include <openssl/asn1.h>

int main(void)
{
    int ret, len, indent;
    BIO *bp;
    char buf[5000];
    FILE *fp;

    bp = BIO_new(BIO_s_file());
    BIO_set_fp(bp, stdout, BIO_NOCLOSE);

    fp = fopen("prikey.pem", "rb");
    len = fread(buf, 1, 5000, fp);
    fclose(fp);

    {
        ret = BIO_dump(bp, buf, len);
    }

    {
        indent = 5;
        ret = BIO_dump_indent(bp, buf, len, indent);
    }

    {
        fp = fopen("prikey2.pem", "wb");
        ret = BIO_dump_fp(fp, buf, len);
        fclose(fp);
    }

    BIO_free(bp);
    return 0;
}
