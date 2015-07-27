//============================================================================
// Name        : CExercise.cpp
// Author      : Haier
// Version     : 0.1
// Copyright   : Your copyright notice
// Description : Connect Mysql in C++, Ansi-style
//============================================================================

#include <iostream>
#include <iomanip>
#include <cstring>
#include <fstream>
#include <winsock.h>
#include "mysql.h"
using namespace std;

int main()
{
    MYSQL mysql;
    MYSQL_RES *result;
    MYSQL_ROW row;
    unsigned int i;
    fstream file;
    char test[800];

    file.open("test.txt",ios::out|ios::in);
    if(!file.is_open())
    {
        cout<<"open file failed !"<<endl;
        return 0;
    }

    mysql_init(&mysql);
    if(!mysql_real_connect(&mysql,"10.87.30.1X","root","XXXXX","mysql",3306,NULL,0))
    {
        cout<<mysql_error(&mysql)<<endl;
        cout<<"mysql connect failed !"<<endl;
        return 0;
    }

    mysql_query(&mysql,"set names gbk");
    if(mysql_real_query(&mysql,"select * from user",(unsigned long)strlen("select * from user")))
    {
        cout<<"select failed !"<<endl;
        return 0;
    }

    if(NULL==(result=mysql_store_result(&mysql)))
    {
        cout<<"stroe result failed !"<<endl;
        return 0;
    }

    cout<<mysql_num_fields(result)<<endl;
    while((row=mysql_fetch_row(result)))
    {
        for(i=0; i<15; i++)
        {
            file<<setw(50)<<setiosflags(ios::left)<<row[i];;
        }
        file<<endl;
    }

    mysql_free_result(result);
    mysql_close(&mysql);

    file.seekg(0);
    while(!file.eof())
    {
        file.getline(test,800);
        cout<<test<<endl;
    }

    file.close();
    return 0;
}

