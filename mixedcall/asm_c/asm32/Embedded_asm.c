#include <stdio.h>

int main(void) {
     long eax=4;
     long ebx=2;

     __asm__ __volatile__ ("addl %1, %0"
             : "=b"((long)ebx)
             : "a"((long)eax), "b"((long)ebx)
             : "1"
             );

     printf("ebx=%d\n", ebx);
     return 0;
}
