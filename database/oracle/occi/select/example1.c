#include <iostream>
#define LINUXOCCI //避免函数重定义错误
#include <occi.h>
using namespace std;
using namespace oracle::occi;
int main()
{
    Environment *env=Environment::createEnvironment(Environment::DEFAULT);
    cout<<"success"<<endl;
    string name = "so1";
    string pass = "1qaz!QAZ";

    try
    {
        Connection *conn = env->createConnection(name, pass);
        cout<<"conn success"<<endl;
        env->terminateConnection(conn);
    }
    catch(SQLException e)
    {
        cout<<e.what()<<endl;
        return -1;
    }
    Environment::terminateEnvironment(env);
    cout<<"end!"<<endl;
    return 0;
}
