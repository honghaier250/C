#include <linux/module.h> //模块所需的大量符号和函数定义
#include <linux/init.h> //指定初始化和清除函数
#include <linux/fs.h> //文件系统相关的函数和头文件
#include <linux/cdev.h> //cdev结构的头文件
#include <asm/uaccess.h> //在内核和用户空间中移动数据的函数
#include <linux/slab.h>
#include <linux/device.h>

MODULE_LICENSE("GPL"); //指定代码使用的许可证

//文件操作函数的声明
int my74hc595_open(struct inode *, struct file *);
int my74hc595_release(struct inode *, struct file *);
ssize_t my74hc595_read(struct file *, char *, size_t, loff_t *);
ssize_t my74hc595_write(struct file *, const char *, size_t, loff_t *);

int dev_major = 1253; //指定主设备号
int dev_minor = 0; //指定次设备号

static struct class *firstdrv_class;
static struct device *firstdrv_class_dev;

struct cdev *my74hc595_cdev; //内核中表示字符设备的结构
int *gp_testdata;//测试用数据

struct file_operations my74hc595_fops= //将文件操作与分配的设备号相连
{
owner:
THIS_MODULE, //指向拥有该模块结构的指针
open:
my74hc595_open,
release:
my74hc595_release,
read:
my74hc595_read,
write:
my74hc595_write,
};

static void __exit my74hc595_exit(void) //退出模块时的操作
{
dev_t devno=MKDEV(dev_major, dev_minor); //dev_t是用来表示设备编号的结构

cdev_del(my74hc595_cdev); //从系统中移除一个字符设备
kfree(my74hc595_cdev); //释放自定义的设备结构
kfree(gp_testdata);
unregister_chrdev_region(devno, 1); //注销已注册的驱动程序

device_unregister(firstdrv_class_dev); //删除/dev下对应的字符设备节点
class_destroy(firstdrv_class);

printk("my74hc595 unregister success\n");
}

static int __init my74hc595_init(void) //初始化模块的操作
{
int ret, err;
dev_t devno;
#if 1
//动态分配设备号，次设备号已经指定
ret=alloc_chrdev_region(&devno, dev_minor, 1, "my74hc595");
//保存动态分配的主设备号
dev_major=MAJOR(devno);

#else
//根据期望值分配设备号
devno=MKDEV(dev_major, dev_minor);
ret=register_chrdev_region(devno, 1, "my74hc595");
#endif

if(ret<0)

{
printk("my74hc595 register failure\n");
//my74hc595_exit(); //如果注册设备号失败就退出系统
return ret;

}
else

{
printk("my74hc595 register success\n");

}

gp_testdata = kmalloc(sizeof(int), GFP_KERNEL);
#if 0//两种初始化字符设备信息的方法
my74hc595_cdev = cdev_alloc();//调试时，此中方法在rmmod后会出现异常，原因未知
my74hc595_cdev->ops = &my74hc595_fops;
#else
my74hc595_cdev = kmalloc(sizeof(struct cdev), GFP_KERNEL);
cdev_init(my74hc595_cdev, &my74hc595_fops);
#endif

my74hc595_cdev->owner = THIS_MODULE; //初始化cdev中的所有者字段

err=cdev_add(my74hc595_cdev, devno, 1); //向内核添加这个cdev结构的信息
if(err<0)
printk("add device failure\n"); //如果添加失败打印错误消息

firstdrv_class = class_create(THIS_MODULE, "my74hc595");
firstdrv_class_dev = device_create(firstdrv_class, NULL, MKDEV(dev_major, 0), NULL,"my74hc595-%d", 0);//在/dev下创建节点

printk("register my74hc595 dev OK\n");

return 0;
}
//打开设备文件系统调用对应的操作
int my74hc595_open(struct inode *inode, struct file *filp)
{
//将file结构中的private_data字段指向已分配的设备结构
filp->private_data = gp_testdata;
printk("open my74hc595 dev OK\n");
return 0;
}
//关闭设备文件系统调用对应的操作
int my74hc595_release(struct inode *inode, struct file *filp)
{
printk("close my74hc595 dev OK\n");
return 0;
}
//读设备文件系统调用对应的操作
ssize_t my74hc595_read(struct file *filp, char *buf, size_t len, loff_t *off)
{
//获取指向已分配数据的指针
unsigned int *p_testdata = filp->private_data;
//将设备变量值复制到用户空间
if(copy_to_user(buf, p_testdata, sizeof(int)))

{
return -EFAULT;

}
printk("read my74hc595 dev OK\n");
return sizeof(int); //返回读取数据的大小
}
//写设备文件系统调用对应的操作
ssize_t my74hc595_write(struct file *filp, const char *buf, size_t len, loff_t *off)
{
//获取指向已分配数据的指针
unsigned int *p_testdata = filp->private_data;
//从用户空间复制数据到内核中的设备变量
if(copy_from_user(p_testdata, buf, sizeof(int)))

{
return -EFAULT;

}
printk("write my74hc595 dev OK\n");
return sizeof(int); //返回写数据的大小
}

module_init(my74hc595_init); //模块被装载时调用my74hc595_init
module_exit(my74hc595_exit); //模块被卸载时调用my74hc595_exit
