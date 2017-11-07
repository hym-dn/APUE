#include"../../apue.h"
#include<fcntl.h>
#include<sys/mman.h>

//迭代次数
#define NLOOPS 1000
//共享内存区域的尺寸
#define SIZE sizeof(long)

//自增整数，并返回自增前的值
static int update(long *ptr){
    return((*ptr)++);
}

//入口函数
int main(void){
    //记录/dev/zero文件标识符
    int fd;
    //迭代索引
    int i;
    //计数器
    int counter;
    //进程ID
    pid_t pid;
    //映射区域首地址
    void *area;
    //打开/dev/zero文件
    if((fd=open("/dev/zero",O_RDWR))<0){
        err_sys("open error");
    }
    //映射/dev/zero文件
    if((area=mmap(0,SIZE,PROT_READ|PROT_WRITE,
        MAP_SHARED,fd,0))==MAP_FAILED){
        err_sys("mmap error");
    }
    //关闭/dev/zero文件
    close(fd);
    //建立父子进程通信
    TELL_WAIT();
    //创建子进程
    if((pid=fork())<0){
        err_sys("fork error");
    }
    //父进程
    else if(pid>0){
        //循环
        for(i=0;i<NLOOPS;i+=2){
            //自增共享内存中的值
            if((counter=update((long *)area))!=i){
                err_quit("parent: expected %d,got %d",i,counter);
            }
            //通知子进程
            TELL_CHILD(pid);
            //等待子进程通知
            WAIT_CHILD();
        }
    }
    //子进程
    else{
        //迭代
        for(i=1;i<NLOOPS+1;i+=2){
            //等待父进程
            WAIT_PARENT();
            //增共享内存中的值
            if((counter=update((long *)area))!=i){
                err_quit("child: expected %d,got %d",i,counter);
            }
            //通知父进程
            TELL_PARENT(getpid());
        }
    }
    //退出
    exit(0);
}
