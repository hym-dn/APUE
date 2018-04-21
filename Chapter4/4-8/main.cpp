#include"../../apue.h"

int main(void){
    // 更改/tmp为当前工作目录
    if(chdir("/tmp")<0){
        err_sys("chdir failed");
    }
    // 打印消息
    printf("chdir to /tmp succeeded\n");
    // 进程退出
    exit(0);
}