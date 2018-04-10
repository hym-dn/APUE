#include"../../apue.h"

#define BUFFSIZE 4096

int main(void){
    int n;
    char buf[BUFFSIZE];
    // 从标准输入读取内容
    while((n=read(STDIN_FILENO,buf,BUFFSIZE))>0){
        // 向标准输出写
        if(write(STDOUT_FILENO,buf,n)!=n){
            err_sys("write error"); // 写失败
        }
    }
    // 读失败
    if(n<0){
        err_sys("read error");
    }
    // 退出
    exit(0);
}