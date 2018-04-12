#include"../../apue.h"
#include<fcntl.h>

int main(int argc,char *argv[]){
    int val;
    // 如果参数并非两个
    if(argc!=2){
        err_quit("usage: a.out <descriptor#>");
    }
    // 获取文件描述符状态标志
    if((val=fcntl(atoi(argv[1]),F_GETFL,0))<0){
        err_sys("fcntl error for fd %d",atoi(argv[1]));
    }
    // 分析文件描述符读、写状态
    switch(val&O_ACCMODE){
    case O_RDONLY:
        printf("read only");
        break;
    case O_WRONLY:
        printf("write only");
        break;
    case O_RDWR:
        printf("read write");
        break;
    default:
        err_dump("unknown access mode");
    }
    // 检验追加标记
    if(val&O_APPEND){
        printf(",append");
    }
    // 检验非阻塞标记
    if(val&O_NONBLOCK){
        printf(",noblocking");
    }
    // 检验同步标记
#if defined(O_SYNC)
    if(val&O_SYNC){
        printf(",synchronous writes");
    }
#endif
#if !defined(_POSIX_C_SOURCE)&&defined(O_FSYNC)
    if(val&O_FSYNC){
        printf(",synchronous writes");
    }
#endif
    // 输出回车
    putchar('\n');
    // 退出
    exit(0);
}