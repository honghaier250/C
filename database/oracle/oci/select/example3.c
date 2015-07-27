//============================================================================
// Name        : CExercise.cpp
// Author      : Haier
// Version     : 0.1
// Copyright   : Your copyright notice
// Description : Connect Oracle in C++ by oci, Ansi-style
//============================================================================


#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <oci.h>
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
    oclose(&cda);
    ologof(&lda);
    exit(1);
}

sword Login()
{
    char username[20];
    char password[20] ;
    int  times=0;

    do
    {
        if(++times>3)
        {
            return 1;
        }

        cout<<"Enter user-name:";
        cin>>username;
        cout<<"Enter password:";
        cin>>password;

    }
    while(olon( &lda,(OraText*)username, -1, (OraText*)password, -1, 0));

    return 0;
}

int main()
{

    char bill_id[20];
    char password[20];
    char sql[256] = "select bill_id,password from so1.ins_prod_a where bill_id='15238075968'";

    //登陆oracle
    if(Login())
    {
        oci_error();
    }

    //禁止自动提交
    if(ocof(&lda))
    {
        oci_error();
    }

    //打开curse
    if(oopen(&cda,&lda,(OraText*)0,-1,-1,(OraText*)0,-1))
    {
        oci_error();
    }

    //分析sql
    if(oparse(&cda,(OraText*)sql,-1,0,2))
    {
        oci_error();
    }

    //定义输入
    if(odefin(&cda,1,(ub1 *)bill_id,sizeof(bill_id),5,-1,(sb2 *)0,(OraText *)0,0,-1,(ub2 *)0,(ub2 *)0))
    {
        oci_error();
    }

    if(odefin(&cda,2,(ub1 *)password,sizeof(password),5,-1,(sb2 *)0,(OraText *)0,0,-1,(ub2 *)0,(ub2 *)0))
    {
        oci_error();
    }

    //执行sql
    if(oexn( &cda, 1, 0 ))
    {
        oci_error();
    }

    //提取结果
    if(ofen(&cda,1))
    {
        oci_error();
    }

    //输出结果
    cout<<"\n"<<bill_id<<"\t"<<password<<endl;

    //关闭curse
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

