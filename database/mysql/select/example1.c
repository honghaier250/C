//============================================================================
// Name        : CExercise.cpp
// Author      : Haier
// Version     : 0.1
// Copyright   : Your copyright notice
// Description : Connect Mysql in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <string.h>
#include <linux/socket.h>
#include "mysql.h"

int main()
{
    MYSQL mysql;
    MYSQL_RES *result;
    MYSQL_ROW row;

    mysql_init(&mysql);
    if(!mysql_real_connect(&mysql,"127.0.0.1","root","admin","xdgw",3306,NULL,0))
    {
        fprintf(stderr, "%s" ,"mysql connect failed !");
        return 0;
    }

    if(mysql_real_query(&mysql,"SELECT OBJECTID,NAME,ID_CARD,PAPERS FROM TB_PMS_SUBJECT_RES WHERE CERT_SN='48798gdf56sd23tg1'",(unsigned long)strlen("SELECT OBJECTID,NAME,ID_CARD,PAPERS FROM TB_PMS_SUBJECT_RES WHERE CERT_SN='48798gdf56sd23tg1'")))
    {
        fprintf(stderr, "%s" , "select failed !");
        return 0;
    }

    /* if(NULL==(result=mysql_store_result(&mysql))) */
    if(NULL==(result=mysql_use_result(&mysql)))
    {
        fprintf(stderr, "%s" , "stroe result failed !");
        return 0;
    }

    while(row=mysql_fetch_row(result))
    {
        fprintf(stdout, "%s\t%s\t%s\t%s\n" ,row[0], row[1], row[2], row[3]);
    }

    mysql_free_result(result);
    mysql_close(&mysql);

    return 0;
}


