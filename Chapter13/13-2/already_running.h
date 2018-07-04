#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<syslog.h>
#include<string.h>
#include<errno.h>
#include<stdio.h>
#include<sys/stat.h>

#define LOCKFILE "/var/run/daemon.pid" // 定义锁文件
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH) // 锁模式

// 锁指定文件
extern int lockfile(int);

int already_running(void){
    int fd;
    char buf[16];
    // 打开锁文件
    fd=open(LOCKFILE,O_RDWR|O_CREAT,LOCKMODE);
    if(fd<0){ // 打开失败
        // 系统日志
        syslog(LOG_ERR,"can't open %s: %s",LOCKFILE,strerror(errno));
        // 退出
        exit(1);
    }
    // 锁住文件
    if(lockfile(fd)<0){
        // 锁冲突
        if(errno==EACCES||errno==EAGAIN){
            // 关闭锁文件
            close(fd);
            // 返回
            return(1);
        }
        // 书写系统日志
        syslog(LOG_ERR,"can't lock %s: %s",LOCKFILE,strerror(errno));
        // 退出
        exit(1);
    }
    // 修改文件大小
    ftruncate(fd,0);
    // 写文件内容
    sprintf(buf,"%ld",(long)getpid());
    write(fd,buf,strlen(buf)+1);
    // 返回
    return(0);
}