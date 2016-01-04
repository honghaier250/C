#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <setjmp.h>
#include "block.h"

int main(int argc, char **argv)
{
    init_for_globle();            //init for globle
    print_mode_shape();            //print first diamond
    print_next();                //print next diamond
    setitimer(ITIMER_REAL, &level_01, NULL);    //init one leve ;interval 800ms
    signal(SIGALRM, move_down);    //diamond down base on the interval time
    signal(SIGWINCH, print_start_interface);
    key_control();                //using zhe key to play games

    return 0;
}
