#include"../../apue.h"

int glob=6; /* external variable in initialized data */

char buf[]="a write to stdout\n";

int main(void){
    int var; /* automatic variable on the stack */
    pid_t pid;
    var=88;
    // 打印相关信息
    printf("before fork\n"); /* we don't flush stdout */
    // 创建子进程
    if((pid=vfork())<0){ // 失败
        err_sys("fork error");
    }else if(pid==0){ // 子进程 /* child */
        glob++; /* modify variables */
        var++;
        _exit(0);
    }
    // 打印相关信息
    printf("pid=%d,glob=%d,var=%d\n",getpid(),glob,var);
    // 退出
    exit(0);
}