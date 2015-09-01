#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//函 数 名：AscToHex()
//功能描述：把ASCII转换为16进制

unsigned char AscToHex(unsigned char aHex)
{

    if((aHex>=0)&&(aHex<=9))
        aHex += 0x30;
    else if((aHex>=10)&&(aHex<=15))//A-F
        aHex += 0x37;
    else aHex = 0xff;

    return aHex;
}

//函 数 名：HexToAsc()
//功能描述：把16进制转换为ASCII
unsigned char HexToAsc(unsigned char aChar)
{
    if((aChar>=0x30)&&(aChar<=0x39))
        aChar -= 0x30;
    else if((aChar>=0x41)&&(aChar<=0x46))//大写字母
        aChar -= 0x37;
    else if((aChar>=0x61)&&(aChar<=0x66))//小写字母
        aChar -= 0x57;
    else aChar = 0xff;

    return aChar;
}

int main(int argc, char* argv[])
{
    char *data="hello";
    char *p=data;
    FILE *fp;

    fp=fopen("file.hex","wb");

    while(*p)
    {
        printf("%x\n",*p);
        fprintf(fp,"%x ",*p);
        p++;
    }

    fclose(fp);

    return 0;
}
