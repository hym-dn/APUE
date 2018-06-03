#include"../../apue.h"

// 声明信号捕捉函数
static void sig_usr(int); /* one handler for both signals */

int main(void){
    // 安装信号捕捉程序
    if(signal(SIGUSR1,sig_usr)==SIG_ERR){
        err_sys("can't catch SIGUSR1");
    }
    // 安装信号捕捉程序
    if(signal(SIGUSR2,sig_usr)==SIG_ERR){
        err_sys("can't catch SIGUSR2");
    }
    // 无限循环
    for(;;){
        // 使调用进程在接到一个信号前挂起
        pause();
    }
}

// 信号捕捉函数
static void sig_usr(int signo){ /* argument is signal number */
    // 第一用户信号
    if(signo==SIGUSR1){
        printf("received SIGUSR1\n");
    }
    // 第二用户信号
    else if(signo==SIGUSR2){
        printf("received SIGUSR2\n");
    }
    // 其它信号
    else{
        err_dump("received signal %d\n",signo);
    }
}