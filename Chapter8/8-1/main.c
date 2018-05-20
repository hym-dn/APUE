#include"../../apue.h"

int glob=6; /* external variable in initialized data */

char buf[]="a write to stdout\n";

int main(void){
    int var; /* automatic variable on the stack */
    pid_t pid;
    var=88;
    // 向标准输出中写入内容
    if(write(STDOUT_FILENO,buf,sizeof(buf)-1)!=sizeof(buf)-1){
        err_sys("write error");
    }
    // 打印相关信息
    printf("before fork\n"); /* we don't flush stdout */
    // 创建子进程
    if((pid=fork())<0){ // 失败
        err_sys("fork error");
    }else if(pid==0){ // 子进程 /* child */
        glob++; /* modify variables */
        var++;
    }else{ // 父进程
        sleep(2);
    }
    // 打印相关信息
    printf("pid=%d,glob=%d,var=%d\n",getpid(),glob,var);
    // 退出
    exit(0);
}