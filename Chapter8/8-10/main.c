#include"../../apue.h"
#include<sys/wait.h>

int main(void){
    // 进程ID
    pid_t pid;
    // 创建子进程
    if((pid=fork())<0){ // 失败
        err_sys("fork error");
    }
    // 子进程
    else if(pid==0){
        // 执行解释器文件
        if(execl("/home/sar/bin/testinterp",
            "testinterp","myarg1","My ARG2",(char *)0)<0){ // 失败
            err_sys("execl error");
        }
    }
    // 等待子进程结束
    if(waitpid(pid,NULL,0)<0) /* parent*/
        err_sys("waitpid error");
    // 退出
    exit(0);
}