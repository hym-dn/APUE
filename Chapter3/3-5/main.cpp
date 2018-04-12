#include"../../apue.h"
#include<fcntl.h>

void set_fl(int fd,int flags){
    int val;
    // 获取文件描述符状态标志
    if((val=fcntl(fd,F_GETFL,0))<0){
        err_sys("fcntl F_GETFL error");
    }
    // 开启标志
    val|=flags; /* turn on flags */
    // 设置标志
    if(fcntl(fd,F_SETFL,val)<0){
        err_sys("fcntl F_SETFL error");
    }
}