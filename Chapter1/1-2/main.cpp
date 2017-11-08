#include"../../apue.h"

//缓冲区尺寸
#define BUFFSIZE 4096

int main(void){
    //字节个数
    int n;
    //缓冲区
    char buf[BUFFSIZE];
    //循环从标准输入读取，直到读取结束
    while((n=read(STDIN_FILENO,buf,BUFFSIZE))>0){
        if(write(STDOUT_FILENO,buf,n)!=n){
            err_sys("write error");
        }
    }
    //如果读取失败
    if(n<0){
        err_sys("read error");
    }
    //退出
    exit(0);
}
