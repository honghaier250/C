#include<stdio.h>
#include<sys/types.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include<termios.h>

int main()
{
    struct winsize old_window_size;
    struct winsize new_window_size;

    new_window_size.ws_col=197;
    new_window_size.ws_row=9;

    //获得原始屏幕大小
    ioctl(STDIN_FILENO,TIOCGWINSZ,&old_window_size);
    printf("%d\n",old_window_size.ws_col);
    printf("%d\n",old_window_size.ws_row);

    //设置新大小
    ioctl(STDIN_FILENO,TIOCSWINSZ,&new_window_size);

    return 0;
}
