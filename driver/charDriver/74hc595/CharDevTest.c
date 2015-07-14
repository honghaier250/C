#include<sys/types.h>
#include<sys/stat.h>
#include<stdio.h>
#include<fcntl.h>
#include<string.h>

main()
{
    int fd;
    long long count=10000000000000;
    char num[250]="performance test";
    //fd=open("/dev/my74hc595",O_RDWR,S_IRUSR|S_IWUSR);//可读写方式打开设备文件
    fd=open("/dev/my74hc595-0",O_RDWR);//可读写方式打开设备文件

    do
    {
        if(fd!=-1)
        {
            read(fd,&num,sizeof(num));//读取设备变量
            printf("The my74hc595 is %s\n",num);

            //printf("Please input the num written to my74hc595\n");
            //scanf("%s",&num);
            write(fd,&num,strlen(num));//写设备变量

            read(fd,&num,sizeof(num));//再次读取刚才写的值
            printf("The my74hc595 is %s\n",num);

            close(fd);//关闭设备文件
        }
        else
        {
            printf("Device open failure\n");
            perror("open my74hc595");
        }
    }while(count--);

    return 0;
}
