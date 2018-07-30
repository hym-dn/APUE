#include"apue.h"
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



}