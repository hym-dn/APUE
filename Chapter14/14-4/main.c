#include"../../apue.h"
#include"../../Chapter10/10-17/tell.h"
#include<fcntl.h>

// 锁一个字节
static void lockabyte(const char *name,int fd,off_t offset){
    // 加锁
    if(writew_lock(fd,offset,SEEK_SET,1)<0){
        err_sys("%s: writew_lock error",name);
    }
    // 打印
    printf("%s: got the lock,byte %ld\n",name,offset);
}

// 入口函数
int main(void){
    int fd;
    pid_t pid;
    /**
     * Create a file and write two bytes to it.
     */
    // 创建文件
    if((fd=creat("templock",FILE_MODE))<0){
        err_sys("creat error");
    }
    // 写入两个字节
    if(write(fd,"ab",2)!=2){
        err_sys("write error");
    }
    // 初始化父子进程通讯环境
    TELL_WAIT();
    // 创建子进程
    if((pid=fork())<0){
        err_sys("fork error");
    }
    // 子进程
    else if(pid==0){ /* child */
        // 锁一个字节
        lockabyte("child",fd,0);
        // 通知父亲
        TELL_PARENT(getppid());
        // 等待父亲响应
        WAIT_PARENT();
        // 锁一个字节
        lockabyte("child",fd,1);
    }
    // 父进程
    else{ /* parent */
        // 锁一个字节
        lockabyte("parent",fd,1);
        // 通知子进程
        TELL_CHILD(pid);
        // 等待子进程
        WAIT_CHILD();
        // 锁一个字节
        lockabyte("parent",fd,0);
    }
    // 退出
    exit(0);
}