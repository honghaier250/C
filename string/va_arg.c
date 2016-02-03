#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/*函数原型声明，至少需要一个确定的参数，注意括号内的省略号*/
int demo(char *msg, ...);

int main(void)
{
    demo("DEMO", "This", "is", "a", "demo!", "");

    return 0;
}

/*ANSI标准形式的声明方式，括号内的省略号表示可选参数*/
int demo(char *msg, ...)
{
    va_list argp;                /*第一步：定义保存函数参数的结构 */
    int argno = 0;
    char *para;

    va_start(argp, msg);        /*第二步：argp指向传入的第一个可选参数，msg是最后一个确定的参数 */

    while (1)
    {

        para = va_arg(argp, char *);    /*第三步：指向取出形参，后面的char为参数的类型 */
        if (strcmp(para, "") == 0)
            break;

        printf("Parameter #%d is: %s\n", argno, para);
        argno++;
    }

    va_end(argp);                /*第四步：将argp置为NULL */

    return 0;
}
