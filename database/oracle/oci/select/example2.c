//============================================================================
// Name        : CExercise.cpp
// Author      : Haier
// Version     : 0.1
// Copyright   : Your copyright notice
// Description : Connect Oracle in C++, Ansi-style
//============================================================================

#include <oci.h>
#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
using namespace std;

//存放查询数据的结构体
struct result
{
    char ename[20];
    char cname[20];
    result()
    {
        memset(ename, '\0', sizeof(ename));
        memset(cname, '\0', sizeof(cname));
    }
};

int main()
{
    // 初始化 OCI 环境句柄指针
    OCIEnv *envhpp = NULL;
    // 初始化服务器句柄
    OCIServer *servhpp = NULL;
    // 用于捕获 OCI 错误信息
    OCIError *errhpp = NULL;
    // 初始化会话句柄
    OCISession *usrhpp = NULL;
    // 初始化服务上下文句柄
    OCISvcCtx *svchpp = NULL;
    // 初始化表达式句柄
    OCIStmt *stmthpp = NULL;

    string server="KFCS";

    // 创建 OCI 环境 , 并设置环境句柄。
    sword swResult = OCIEnvCreate(&envhpp, OCI_DEFAULT, NULL, NULL, NULL, NULL, 0, NULL);
    if (swResult != OCI_SUCCESS && swResult != OCI_SUCCESS_WITH_INFO)
    {
        cout << swResult<<"Oracle environment initialization error!" << endl;
        exit(1);
    }
    cout << "Oracle environment initialization success!" << endl;

    // 创建错误句柄
    OCIHandleAlloc((dvoid *)envhpp, (dvoid **)&errhpp, OCI_HTYPE_ERROR, (size_t)0, (dvoid **)0);

    // 创建服务句柄
    OCIHandleAlloc((dvoid *)envhpp, (dvoid **)&servhpp, OCI_HTYPE_SERVER, (size_t)0, (dvoid **)0);

    // 连接服务器，如果失败则获取错误码
    if (OCIServerAttach(servhpp, errhpp, (text *)server.c_str(), strlen(server.c_str()), 0) != OCI_SUCCESS)
    {
        int errcno;
        char errbuf[512] = "";
        sb4 errcode;

        // 获取错误指针和 OCI 错误代码
        OCIErrorGet((dvoid *)errhpp, (ub4)1, (text *)NULL, &errcode, (ub1 *)errbuf, (ub4)sizeof(errbuf), OCI_HTYPE_ERROR);
        errcno = errcode;

        cout << "Oracle server attach error:" << errbuf << endl;
        OCIHandleFree((dvoid *)envhpp,OCI_HTYPE_ENV);
        OCIHandleFree((dvoid *)servhpp,OCI_HTYPE_SERVER);
        OCIHandleFree((dvoid *)errhpp,OCI_HTYPE_ERROR);
        exit(1);
    }
    cout << "Oracle server attach success!"<< endl;

    /***************** 连接数据库 ****************/
    string user = "so1";
    string pas = "1qaz!QAZ";
    errhpp = NULL;

    // 创建错误句柄
    (void) OCIHandleAlloc((dvoid *)envhpp, (dvoid **)&errhpp, OCI_HTYPE_ERROR, (size_t)0, (dvoid **)0);
    // 创建服务上下文句柄
    (void) OCIHandleAlloc((dvoid *)envhpp, (dvoid **)&svchpp, OCI_HTYPE_SVCCTX, (size_t) 0, (dvoid **)0);
    // 设置属性
    (void) OCIAttrSet((dvoid *)svchpp, OCI_HTYPE_SVCCTX, (dvoid *)servhpp, (ub4)0, OCI_ATTR_SERVER, (OCIError *)errhpp);
    // 创建用户连接句柄
    (void) OCIHandleAlloc((dvoid *)envhpp, (dvoid **)&usrhpp, (ub4)OCI_HTYPE_SESSION, (size_t) 0, (dvoid **)0);
    // 设置用户名、密码
    (void) OCIAttrSet((dvoid *)usrhpp, (ub4)OCI_HTYPE_SESSION, (dvoid *)user.c_str(), (ub4)strlen(user.c_str()), (ub4)OCI_ATTR_USERNAME, errhpp);
    (void) OCIAttrSet((dvoid *)usrhpp, (ub4)OCI_HTYPE_SESSION, (dvoid *)pas.c_str(), (ub4)strlen(pas.c_str()), (ub4)OCI_ATTR_PASSWORD, errhpp);

    // 创建会话连接
    if(OCISessionBegin(svchpp, errhpp, usrhpp, OCI_CRED_RDBMS, (ub4)OCI_DEFAULT) != OCI_SUCCESS)
    {
        int errcno;
        char errbuf[512]= {'\0'};
        sb4 errcode;

        // 获取错误指针和 OCI 错误代码
        OCIErrorGet((dvoid *)errhpp, (ub4)1, (text *)NULL, &errcode, (ub1 *)errbuf, (ub4)sizeof(errbuf), OCI_HTYPE_ERROR);
        errcno = errcode;
        cout << "User session error:" << errbuf << endl;
        OCIHandleFree((dvoid *)errhpp,OCI_HTYPE_ERROR);
        OCIHandleFree((dvoid *)usrhpp,OCI_HTYPE_SESSION);
        OCIHandleFree((dvoid *)svchpp,OCI_HTYPE_SVCCTX);
        exit(1);
    }
    cout << "user session success!" << endl;

    (void) OCIAttrSet((dvoid *)svchpp, (ub4) OCI_HTYPE_SVCCTX, (dvoid *)usrhpp, (ub4)0, (ub4)OCI_ATTR_SESSION, errhpp);

    /*************** 执行 查询SQL 语句 ******************/
    errhpp = NULL;

    // 创建一个表达式句柄
    if (OCIHandleAlloc((dvoid *)envhpp, (dvoid **)&stmthpp, OCI_HTYPE_STMT, (size_t)0, (dvoid **)0) != OCI_SUCCESS)
    {
        cout << "Create STMT error !" << endl;
        exit(1);
    }
    cout << "Create stmt success !" << endl;

    // 创建错误句柄
    OCIHandleAlloc((dvoid *)envhpp, (dvoid **)&errhpp, OCI_HTYPE_ERROR, (size_t)0, (dvoid **)0);

    // Select语句
    char sql[255] = "Select t.Bill_Id,t.Password from So1.Ins_Prod_a t ";

    if (OCIStmtPrepare(stmthpp, errhpp, (text *)sql, (ub4)strlen(sql), (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT) != OCI_SUCCESS)
    {
        cout << "Create prepare error!" << sql << endl;
        exit(1);
    }
    cout << "Create prepare success!" << endl;

    /********* 绑定参数 ***********/
    // 申请绑定字段的句柄
    OCIDefine *bhp1 = NULL;
    OCIDefine *bhp2 = NULL;

    // 存放数据的结构
    struct result rst;

    // 指定提取数据长度
    ub2 datalen = 0;
    // 定义指示器变量 , 用于取可能存在空值的字段
    char isnul[6] = "";
    // 定义输出变量 ,
    OCIDefineByPos(stmthpp, &bhp1, errhpp, 1, (dvoid *)&rst.ename, sizeof(rst.ename), SQLT_STR, NULL, &datalen, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthpp, &bhp2, errhpp, 2, (dvoid *)&rst.cname, sizeof(rst.cname), SQLT_STR, NULL, &datalen, NULL, OCI_DEFAULT);

    // 获取 SQL 语句类型
    ub2 stmt_type;
    OCIAttrGet ((dvoid *)stmthpp, (ub4)OCI_HTYPE_STMT, (dvoid *)&stmt_type, (ub4 *)0, (ub4)OCI_ATTR_STMT_TYPE, errhpp);

    // 执行 SQL 语句
    OCIStmtExecute(svchpp, stmthpp, errhpp, (ub4)0, (ub4)0, (OCISnapshot *)NULL, (OCISnapshot *)NULL, OCI_DEFAULT);

    // 获取查询信息
    int rows_fetched;
    do
    {
        cout << rst.ename<< " ";
        cout << rst.cname<< " \n";
    }
    while(OCIStmtFetch2(stmthpp, errhpp, 1, OCI_FETCH_NEXT, 1, OCI_DEFAULT) != OCI_NO_DATA);

    // 获得记录条数
    OCIAttrGet((CONST void *)stmthpp, OCI_HTYPE_STMT, (void *)&rows_fetched, (ub4 *)sizeof(rows_fetched), OCI_ATTR_ROW_COUNT, errhpp);
    cout << " rows :" << rows_fetched << endl;

    /*************** 执行 新增SQL 语句 ******************/
    if (OCIHandleAlloc((dvoid *)envhpp, (dvoid **)&stmthpp, OCI_HTYPE_STMT, (size_t)0, (dvoid **)0) != OCI_SUCCESS)
    {
        cout << "Create STMT error !" << endl;
        exit(1);
    }
    cout << "Create stmt success !" << endl;

    OCIHandleAlloc((dvoid *)envhpp, (dvoid **)&errhpp, OCI_HTYPE_ERROR, (size_t)0, (dvoid **)0);

    // Insert语句
    char sql2[255] = "insert into So1.Ins_Prod_a (Bill_Id, Password) values('12345678901', 'W*_6oJ')";

    // 准备Sql语句
    if (OCIStmtPrepare(stmthpp, errhpp, (text *)sql2, (ub4)strlen(sql2), (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT) != OCI_SUCCESS)
    {
        cout << "Create prepare error!" << sql2 << endl;
        exit(1);
    }
    cout << "Create prepare success!" << endl;

    // 执行SQL 语句
    OCIStmtExecute(svchpp, stmthpp, errhpp, (ub4)1, (ub4)0, (OCISnapshot *)NULL, (OCISnapshot *)NULL, OCI_DEFAULT);

    // 断开用户会话
    OCILogoff(svchpp, errhpp);

    // 断开服务器连接
    OCIServerDetach(servhpp, errhpp, OCI_DEFAULT);

    // 释放资源
    OCIHandleFree((dvoid *) stmthpp, OCI_HTYPE_STMT);
    OCIHandleFree((dvoid *) svchpp, OCI_HTYPE_SVCCTX);
    OCIHandleFree((dvoid *) servhpp, OCI_HTYPE_SERVER);
    OCIHandleFree((dvoid *) errhpp, OCI_HTYPE_ERROR);

    return 0;
}

