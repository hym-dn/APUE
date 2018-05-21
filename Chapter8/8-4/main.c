#include"../../apue.h"
#include"../8-3/pr_exit.h"
#include<sys/wait.h>

int main(void){
    // 子进程ID
    pid_t pid;
    // 子进程终止状态
    int status;
    // 创建子进程
    if((pid=fork())<0){ // 失败
        err_sys("fork error");
    }
    // 子进程
    else if(0==pid){ /* child */
        exit(7);
    }
    // 等待子进程结束，并打印子进程终止状态
    if(wait(&status)!=pid){ /* wait for child */
        err_sys("wait error");
    }
    pr_exit(status); /* and print its status */
    // 创建子进程
    if((pid=fork())<0){ // 失败
        err_sys("fork error");
    }
    // 子进程
    else if(0==pid){ /* child */
        abort(); /* generates SIGABRT */
    }
    // 等待子进程结束，并打印子进程终止状态
    if(wait(&status)!=pid){ /* wait for child */
        err_sys("wait error");
    }
    pr_exit(status); /* and print its status */
    // 创建子进程
    if((pid=fork())<0){ // 失败
        err_sys("fork error");
    }
    // 子进程
    else if(0==pid){ /* child */
        int i=0;
        status/=i; /* divide by 0 generates SIGFPE */
    }
    // 等待子进程结束，并打印子进程终止状态
    if(wait(&status)!=pid){ /* wait for child */
        err_sys("wait error");
    }
    pr_exit(status); /* and print its status */
    // 退出
    exit(0);
}