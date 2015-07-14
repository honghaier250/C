#include <stdio.h>
#include <string.h>
//包函 libiconv库头文件
#include <iconv.h>
//导入 libiconv库

int ChangeCode( const char* pFromCode,
        const char* pToCode,
        const char* pInBuf,
        size_t* iInLen,
        char* pOutBuf,
        size_t* iOutLen );

int main( int argc, char* argv[] )
{
    char sInBuf[100];
    char sOutBuf[100];
    size_t iInLen = 0;
    size_t iOutLen = 100;
    int iRet;
    strcpy( sInBuf, "测试 Test Source" );
    puts(sInBuf);

    memset( sOutBuf, 0x00, 100 );
    iInLen = strlen( sInBuf );
    iRet = ChangeCode( "GBK", "UTF-16", sInBuf, &iInLen, sOutBuf, &iOutLen );
    puts(sOutBuf);
    iRet = ChangeCode( "UTF-16", "GBK", sOutBuf, &iOutLen , sOutBuf, &iOutLen );
    puts(sOutBuf);
    return 0;
}

int ChangeCode( const char* pFromCode,
        const char* pToCode,
        const char* pInBuf,
        size_t* iInLen,
        char* pOutBuf,
        size_t* iOutLen )
{
    int iRet;
    //打开字符集转换
    iconv_t hIconv = iconv_open( pToCode, pFromCode );
    if ( -1 == (int)hIconv )
    {
        return -1;//打开失败，可能不支持的字符集
    }
    //开始转换
    iRet = iconv( hIconv, (const char**)(&pInBuf), iInLen, (char**)(&pOutBuf), iOutLen );
    //关闭字符集转换
    iconv_close( hIconv );
    return iRet;
}
