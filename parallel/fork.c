#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;

    pid=fork();

    if (pid < 0)
        printf("error in fork!");
    else if (pid == 0)
    {
        printf("child process, id: %d\n",getpid());
        exit(0);
    }
    else
    {
        printf("parent process, id: %d\n",getpid());

    }

    while((pid  = waitpid(-1,NULL,WNOHANG))>0);
    printf("successed,id: %d\n", getpid());

    return 0;
}
