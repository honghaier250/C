//============================================================================
// Name        : CExercise.cpp
// Author      : Haier
// Version     : 0.1
// Copyright   : Your copyright notice
// Description : Connect Oracle in C++, Ansi-style
//============================================================================

#include <oci.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <ociapr.h>
#include <ocidem.h>
using namespace std;

Lda_Def lda;
cda_def cda;

dvoid oci_error(void)
{
    text msg[600];
    sword rv;
    //取错误信息
    rv=oerhms(&lda,cda.rc,msg,600);
    //显示错误码和错误信息
    printf("\n\n%.*s",rv,msg);
    //显示发生错误的oci函数
    printf("processing oci function %s\n",oci_func_tab[cda.fc]);
    //关闭光标
    if(oclose(&cda))
        printf("error closing cursor!\n");
    if(ologof(&lda))
        printf("error logging off!\n");

    exit(1);
}


int main()
{
    char szDbUser[]="so1@KFCS";
    char szPassword[] ="1qaz!QAZ";
    char pc_msg[256];
    char sql[256] = "update so1.ins_prod_a set state='1' where bill_id='15238075968'";

    if(olon( &lda,(OraText*)szDbUser, -1, (OraText*)szPassword, -1, 0))
    {
        oci_error();
    }

    cout<<"Connect to Oracle DB Successed!"<<endl;

    if(oopen(&cda,&lda,(OraText*)0,-1,-1,(OraText*)0,-1))
    {
        oci_error();
    }

    cout<<"Open Curse Successed!"<<endl;


    if(oparse(&cda,(OraText*)sql,-1,0,2))
    {
        oci_error();
    }

    cout<<"Parse Sql Successed!"<<endl;

    if(oexn( &cda, 1, 0 ))
    {
        oci_error();
    }

    cout<<"exec Sql Successed!"<<endl;

    if(oparse(&cda,(text*)"commit",-1,0,2))
    {
        oci_error();
    }

    cout<<"Commit Successed!"<<endl;

    if(oclose(&cda))
    {
        printf("error closing cursor!/n");
        exit(1);
    }

    //结束事务，退出oracle
    if(ologof(&lda))
    {
        printf("error logging off!/n");
        exit(1);
    }

    return 0;
}
