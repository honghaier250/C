#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <grp.h>
#include <unistd.h>
#include <errno.h>

static char buf[BUFSIZ];

/* 打印此时进程的各种ID */

void show_id()
{
    uid_t uid = getuid();
    gid_t gid = getgid();
    uid_t euid = geteuid();
    gid_t egid = getegid();

    printf("UID: %d\n", uid);
    printf("GID: %d\n", gid);
    printf("EUID: %d\n", euid);
    printf("EGID: %d\n", egid);
}

int main(int argc, char *argv[])
{
    int fd;
    int n;

    if (argc != 2)
    {
        fprintf(stderr, "Usage %s filename\n", argv[0]);
        return -1;
    }

    show_id();
    /* euid = 0才能执行initgroups函数调用 */

    if (geteuid() == 0)
    {
        if (initgroups("bin", getgid()) == -1)
        {
            perror("initgroups failed");
            return -1;
        }
        /* 由于是set-user-id程序, 还原到进程的真实ID */

        if (setuid(getuid()) == -1)
        {
            perror("setuid failed");
            return -1;
        }
    }

    show_id();
    fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        perror("open failed");
        return -1;
    }

    while ( (n = read(fd, buf, BUFSIZ)) > 0)
    {
        write(STDIN_FILENO, buf, n);
    }

    return 0;
}
