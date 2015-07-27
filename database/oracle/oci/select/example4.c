//============================================================================
// Name        : CExercise.cpp
// Author      : Haier
// Version     : 0.1
// Copyright   : Your copyright notice
// Description : Connect Oracle in C++ by oci, Ansi-style
//============================================================================

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <oci.h>
#include <oratypes.h>
#include <ocidfn.h>
#include <ociapr.h>
#include <ocidem.h>
using namespace std;

#define MAX_BINDS 12
#define MAX_ITEM_BUFFER_SIZE 32
#define MAX_SELECT_LIST_SIZE 12
#define MAX_SQL_IDENTIFIER 31
#define PARSE_NO_DELAY 0
#define PARSE_V7_LNG 2
#define RAWWIDTH 18

struct describe
{
    sb4 dbsize;
    sb2 dbtype;
    sb1 buf[MAX_ITEM_BUFFER_SIZE];
    sb4 buflen;
    sb4 dsize;
    sb2 precision;
    sb2 scale;
    sb2 nullok;
};

struct define
{
    ub1 buf[MAX_ITEM_BUFFER_SIZE];
    float flt_fuf;
    sword int_buf;
    sb2 indp;
    sb2 col_retlen,col_retcode;
};

Lda_Def lda;
Cda_Def cda;
ub1     hda[256];
text    errorMsg[600];
sword stmtLevel;
static text sqlStatement[2048];
static sword sqlFunction;
static sword numwidth = 8;
struct describe desc[MAX_SELECT_LIST_SIZE];
struct define def[MAX_SELECT_LIST_SIZE];
text bindValue[MAX_BINDS][MAX_ITEM_BUFFER_SIZE];

sword connect()
{
    text username[20];
    text password[20];
    sword n;

    printf("Copyright (c) 1982, 2014, Oracle.  All rights reserved.\n");
    printf("Connected to Oracle Database:\n\n");
    for(n=3; --n>=0; )
    {
        setbuf(stdout,NULL);
        printf("Enter user-name: ");
        gets((char*)username);
        printf("Enter password: ");
        gets((char*)password);

        if(orlon(&lda,hda,(OraText*)username,-1,(OraText*)password,-1,-1))
        {
            oerhms(&lda,lda.rc,(OraText*)errorMsg,sizeof(errorMsg));
            printf("%sPlease try again.\n",errorMsg);
        }
        else
            return TRUE;

    }

    printf("Connect failed.Exiting...\n");
    return FALSE;
}

sword getSqlStatement()
{
    text sqlBuf[1024];
    text *cp;

    while(1)
    {
        if(1==(++stmtLevel)){
            *sqlStatement='\0';
            printf("\nSQL>");
        }else{
            printf("%3d>",stmtLevel);
        }

        fflush(stdin);
        gets((char*)sqlBuf);
        if(*sqlBuf=='\0'){
            continue;
        }else
        {
            if(!strncmp((char*)sqlBuf,"exit",4) || !strncmp((char*)sqlBuf,"quit",4))
            {
                return 1;
            }
        }

        if(stmtLevel>1)
        {
            strcat((char*)sqlStatement," ");
        }
        strcat((char*)sqlStatement,(char*)sqlBuf);

        cp = &sqlStatement[strlen((char*)sqlStatement)-1];

        while(isspace(*cp))
        {
            cp--;
        }

        if(*cp==';')
        {
            *cp='\0';
            break;
        }

    }

    return 0;
}

void exitOracle(sword code)
{
    if(oclose(&cda))
    {
        oerhms(&cda,cda.rc,(OraText*)errorMsg,sizeof(errorMsg));
        printf("%s\n",errorMsg);
    }

    if(ologof(&lda))
    {
        oerhms(&lda,lda.rc,(OraText*)errorMsg,sizeof(errorMsg));
        printf("%s\n",errorMsg);
    }

    exit(code);
}

void oci_error(Cda_Def *cda)
{
    char choice;

    oerhms(&lda,lda.rc,(OraText*)errorMsg,sizeof(errorMsg));
    printf("%s\n",errorMsg);

    fprintf(stderr,"Do you want to continue ?[y/n]");
    fscanf(stdin,"%c",&choice);

    if(toupper(choice)!='Y')
    {
        exitOracle(1);
    }

    fputc('\n',stdout);
}

sword bind(Cda_Def *cda,text *sqlStatement)
{
    sword i,inLiteral,n;
    text *cp,*ph;

    for(i=0,inLiteral=0,cp=sqlStatement; *cp && i<MAX_BINDS; cp++)
    {
/*      if(*cp=='\'')
        {
            inLiteral=-n;
        }*/

        if(*cp==':' && !inLiteral)
        {
            for(ph=(++cp),n=0; *cp && (isalnum(*cp) || *cp=='_') && n<MAX_SQL_IDENTIFIER; cp++,n++)
            {

            }

            *cp='\0';
            printf("Enter value for %s:",ph);
            gets((char*)&bindValue[i][0]);

            if(obndrv(cda,(OraText *)ph,-1,(ub1*)bindValue[i],-1,VARCHAR2_TYPE,-1,(sb2*)0,(OraText*)0,-1,-1))
            {
                oci_error(cda);
                return -1;
            }

            i++;
        }
    }

    return i;
}

sword describeDefine(Cda_Def *cda)
{
    sword col,deflen,deftype;
    static ub1 *defptr;

    for(col=0; col<MAX_SELECT_LIST_SIZE; col++)
    {
        desc[col].buflen = MAX_ITEM_BUFFER_SIZE;

        if(odescr(cda,col+1,(sb4*)&desc[col].dbsize,(sb2*)&desc[col].dbtype,(sb1*)(int)desc[col].buf,(sb4*)&desc[col].buflen,(sb4*)desc[col].dsize,(sb2*)(int)desc[col].precision,(sb2*)(int)desc[col].scale,(sb2*)(int)desc[col].nullok))
        {
            if(cda->rc==VAR_NOT_IN_LIST)
            {
                break;
            }else{
                oci_error(cda);
                return -1;
            }
        }

        switch(desc[col].dbtype)
        {
            case NUMBER_TYPE:
            {
                desc[col].dbsize = numwidth;

                if(desc[col].scale != 0)
                {
                    defptr = (ub1*)&def[col].flt_fuf;
                    deflen = (sword)sizeof(float);
                    deftype= FLOAT_TYPE;
                    desc[col].dbtype = FLOAT_TYPE;
                }else{
                    defptr = (ub1*)&def[col].int_buf;
                    deflen = (sword)sizeof(sword);
                    deftype= INT_TYPE;
                    desc[col].dbtype = INT_TYPE;
                }
                break;
            }
            case DATE_TYPE:
            {
                desc[col].dbsize = 9;
                defptr = def[col].buf;
                deflen = desc[col].dbsize > MAX_ITEM_BUFFER_SIZE ? MAX_ITEM_BUFFER_SIZE : desc[col].dbsize+1;
                deftype= STRING_TYPE;
                break;
            }
            case ROWID_TYPE:
            {
                desc[col].dbsize = 18;
                defptr = def[col].buf;
                deflen = desc[col].dbsize > MAX_ITEM_BUFFER_SIZE ? MAX_ITEM_BUFFER_SIZE : desc[col].dbsize+1;
                deftype= STRING_TYPE;
                break;
            }
            default:
            {
                defptr = def[col].buf;
                deflen = desc[col].dbsize > MAX_ITEM_BUFFER_SIZE ? MAX_ITEM_BUFFER_SIZE : desc[col].dbsize+1;
                deftype= STRING_TYPE;
                break;
            }

        }

        if(odefin(cda,col+1,(ub1*)defptr,deflen,deftype,-1,(sb2*)(int)def[col].indp,(OraText*)0,-1,-1,(ub2*)&def[col].col_retlen,(ub2*)(int)def[col].col_retcode))
        {
            oci_error(cda);
            return -1;
        }
    }

    return col;
}

void printHeader(sword ncols)
{
    sword col,n,i;

    for(col=0; col<ncols; col++)
    {
        n=RAWWIDTH -desc[col].buflen;

        if(desc[col].dbtype==FLOAT_TYPE || desc[col].dbtype==INT_TYPE)
        {
            printf("%*.*s",desc[col].buflen,desc[col].buflen,desc[col].buf);
            printf("%*c",n,' ');
        }else{
            printf("%*.*s",desc[col].buflen,desc[col].buflen,desc[col].buf);
            printf("%*c",n,' ');
        }
    }

    fputc('\n',stdout);

    for(col=0; col<ncols; col++)
    {
        n=RAWWIDTH -desc[col].buflen;

        for(i=desc[col].buflen; i; i--)
        {
            fputc('-',stdout);

        }

        if(desc[col].dbtype==FLOAT_TYPE || desc[col].dbtype==INT_TYPE)
        {
            printf("%*c",n,' ');
        }else
        {
            printf("%*c",n,' ');
        }

    }

    fputc('\n',stdout);

}

void printRaw(Cda_Def *cda,sword ncols)
{
    sword col,n;

    while(1)
    {
        fputc('\n',stdout);

        if(ofetch(cda))
        {
            if(cda->rc==NO_DATA_FOUND)
            {
                break;
            }
            if(cda->rc!=NULL_VALUE_RETURNED)
            {
                oci_error(cda);
            }
        }

        for(col=0; col<ncols; col++)
        {

            if(def[col].indp<0)
            {
                printf("%*c",desc[col].dbsize,' ');
            }else{

                switch(desc[col].dbtype)
                {
                    case FLOAT_TYPE:
                    {
                        n = printf("%f",def[col].flt_fuf);
                        n = RAWWIDTH - n;
                        if(n)
                        {
                            printf("%*c",n,' ');
                        }
                        break;
                    }
                    case INT_TYPE:
                    {
                        n = printf("%d",def[col].int_buf);
                        n = RAWWIDTH - n;
                        if(n)
                        {
                            printf("%*c",n,' ');
                        }
                        break;
                    }
                    default:
                    {
                        n = printf("%s",def[col].buf);
                        n = RAWWIDTH - n;
                        if(n)
                        {
                            printf("%*c",n,' ');
                        }
                        break;
                    }
                }
            }

        }
    }
}

int main()
{
    sword ncols;

    //登陆oracle
    if(!connect())
    {
        exit(-1);
    }

    //打开curse
    if(oopen(&cda,&lda,(OraText*)0,-1,-1,(OraText*)0,-1))
    {
        oerhms(&lda,lda.rc,(OraText*)errorMsg,sizeof(errorMsg));
        printf("%s,Please try again.\n",errorMsg);
        exit(-1);
    }

    while(1)
    {
        stmtLevel=0;

        //输入sql
        if(getSqlStatement())
        {
            exitOracle(0);
        }

        //分析sql
        if(oparse(&cda,(OraText*)sqlStatement,-1,PARSE_NO_DELAY,PARSE_V7_LNG))
        {
            oci_error(&cda);
            continue;
        }

        sqlFunction = cda.ft;

        //绑定变量
        if((ncols = bind(&cda,sqlStatement))==-1)
        {
            continue;
        }

        if(sqlFunction==FT_SELECT)
        {
            if((ncols=describeDefine(&cda)) == -1)
            {
                continue;
            }
        }

        //执行sql
        if(oexec(&cda))
        {
            oci_error(&cda);
            continue;
        }

        //若是查询，则显示查询结果;否则，提示事件
        if(sqlFunction==FT_SELECT)
        {
            printHeader(ncols);
            printRaw(&cda,ncols);
        }else{
            if(ocom(&lda))
            {
                oci_error(&lda);
                continue;
            }
        }

        //显示处理行数
        if(sqlFunction==FT_SELECT || sqlFunction==FT_UPDATE || sqlFunction==FT_DELETE || sqlFunction==FT_INSERT)
        {
            printf("\n\n%d row%c processed.\n",cda.rpc,cda.rpc==1 ? '\0' : 's');
        }else{
            printf("\n\nStatement processed.\n");
        }
    }
}

