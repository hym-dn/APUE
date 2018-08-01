#include"../../apue.h"
#include<errno.h>
#include<fcntl.h>
#include<sys/wait.h>

int main(int argc,char *argv[]){
    // 
    int fd;
    pid_t pid;
    char buf[5];
    struct stat statbuf;
    // 如果参数个数非法
    if(argc!=2){
        fprintf(stderr,"usage: %s filename\n",argv[0]);
        exit(1);
    }
    // 打开指定文件
    if((fd=open(argv[1],O_RDWR|O_CREAT|O_TRUNC,FILE_MODE))<0){
        err_sys("open error");
    }
    // 写6个字节
    if(write(fd,"abcdef",6)!=6){
        err_sys("write error");
    }
    // 打开组设置ID位，并且关闭组执行位
    /* turn on set-group-ID and turn off group-execute */
    // 获取文件信息
    if(fstat(fd,&statbuf)<0){
        err_sys("fstat error");
    }
    // 打开组设置ID位，并且关闭组执行位
    if(fchmod(fd,(statbuf.st_mode&~S_IXGRP)|S_ISGID)<0){
        err_sys("fchmod error");
    }
    // 初始化进程通信机制
    TELL_WAIT();
    // 创建父进程
    if((pid=fork())<0){
        err_sys("fork error");
    }
    // 父进程
    else if(pid>0){ /* parent */
        // 在文件中加锁
        /* write lock entire file */
        if(write_lock(fd,0,SEEK_SET,0)<0){
            err_sys("write_lock error");
        }
        // 向子进程发送信号
        TELL_CHILD(pid);
        // 等待子进程结束
        if(waitpid(pid,NULL,0)<0){
            err_sys("waitpid error");
        }
    }
    // 子进程
    else{ /* child */
        // 等待父进程加锁
        WAIT_PARENT(); /* wait for parent to set lock */
        // 设置文件为非阻塞
        set_fl(fd,O_NONBLOCK);
        // 检测读锁
        /* first let's see what error we get if region is locked */
        if(read_lock(fd,0,SEEK_SET,0)!=-1){ /* no wait */
            err_sys("child: read_lock succeeded");
        }
        // 打印信息
        printf("read_lock of already-locked region returns %d\n",errno);
        // 尝试读取
        /* now try to read the mandatory locked file */
        if(lseek(fd,0,SEEK_SET)==-1){
            err_sys("lseek error");
        }
        if(read(fd,buf,2)<0){ // 强制性锁有效
            err_ret("read failed (mandatory locking works)");
        }else{ // 非强制性锁无效
            printf("read OK (no mandatory locking),buf=%2.2s\n",buf);
        }
    }
    // 退出
    exit(0);
}