#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <python2.6/Python.h>

int main(int argc, char*argv[])
{
    Py_Initialize();

    PyRun_SimpleString("print 'Hello_World' ");
    Py_Finalize();

    puts("按任意键退出...");
    system("read -n 1");
    return   0 ;
}
