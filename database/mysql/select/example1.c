//============================================================================
// Name        : CExercise.cpp
// Author      : Haier
// Version     : 0.1
// Copyright   : Your copyright notice
// Description : Connect Mysql in C++, Ansi-style
//============================================================================

#include <iostream>
#include <cstring>
#include <linux/socket.h>
#include <mysql/mysql.h>
using namespace std;

int main()
{
    MYSQL mysql;
    MYSQL_RES *result;
    MYSQL_ROW row;

    mysql_init(&mysql);
    if(!mysql_real_connect(&mysql,"10.87.30.13X","root","XXXXXX","mysql",3306,NULL,0))
    {
        cout<<"mysql connect failed !"<<endl;
        return 0;
    }

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

    while(row=mysql_fetch_row(result))
    {
        cout<<row[0]<<"\t"<<row[1]<<endl;
    }

    mysql_free_result(result);
    mysql_close(&mysql);



    cout << "!!!Hello World\t!!!" << endl; // prints !!!Hello World!!!
    return 0;
}


