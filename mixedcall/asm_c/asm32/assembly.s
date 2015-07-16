 .code32
.section .text
.type a_add,@function
.globl a_add
a_add:
    pushl %ebp        #现场保护
    movl %esp,%ebp

    movl 8(%ebp),%eax #取得C函数传过来的参数
    pushl %ecx        #保护ecx，用作临时变量
    movl (%eax),%ecx  #取得指针所指的内容
    addl $1,%ecx      #将内容+1
    movl %ecx,(%eax)  #将内容放回指针所指的地方
    popl %ecx         #恢复ecx
    pushl %eax        #压桟，以便传参给C函数
    call c_add        #调用C函数

    addl $4,%esp      #清理局部变量
    popl %ebp         #恢复现场
    ret               #返回
