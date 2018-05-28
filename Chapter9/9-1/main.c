#include"../../apue.h"
#include<errno.h>

// 挂断信号处理函数
static void sig_hup(int signo){
    printf("SIGHUP received,pid=%d\n",getpid()); // 打印挂断信号信息
}

// 打印进程信息
static void pr_ids(char *name){
    // 打印进程信息
    printf("%s: pid= %d,ppid= %d,pgrp=%d,tpgrp=%d\n",
        name,getpid(),getppid(),getpgrp(),tcgetpgrp(STDIN_FILENO));
    // 冲洗缓冲
    fflush(stdout);
}

int main(void){
    char c;
    // 进程ID
    pid_t pid;
    // 打印父进程信息
    pr_ids("parent");
    // 创建子进程
    if((pid=fork())<0){
        err_sys("fork error");
    }
    // 父进程
    else if(pid>0){ /* parent */
        // 等待子进程停止
        sleep(5); /* sleep to let child stop itself */
        // 退出
        exit(0); /* then parent exits */
    }
    // 子进程
    else{ /* child */
        // 打印子进程信息
        pr_ids("child");
        // 安装挂断信号处理程序
        signal(SIGHUP,sig_hup); /* establish signal handler */
        // 发送进程停止信号
        kill(getpid(),SIGTSTP); /* stop ourself */
        // 打印子进程信息
        pr_ids("child"); /* prints only if we're continued */
        // 读取标准输入
        if(read(STDIN_FILENO,&c,1)!=1){
            printf("read error from controlling TTY,errno=%d\n",errno);
        }
        // 退出
        exit(0);
    }
}
