#include"../../apue.h"
#include<errno.h>

// 打印屏蔽字
void pr_mask(const char *str){
    sigset_t sigset;
    int errno_save;
    // 存储当前错误信息
    errno_save=errno; /* we can be called by signal handlers */
    // 获取当前信号屏蔽字
    if(sigprocmask(0,NULL,&sigset)<0){
        err_sys("sigprocmask error");
    }
    // 打印信息
    printf("%s",str);
    // 打印信号信息
    if(sigismember(&siget,SIGINT)) printf("SIGINT ");
    if(sigismember(&siget,SIGQUIT)) printf("SIGQUIT ");
    if(sigismember(&siget,SIGUSR1)) printf("SIGUSR1 ");
    if(sigismember(&siget,SIGALRM)) printf("SIGALRM ");
    /* remaining signals can go here */
    printf("\n");
    // 恢复错误编号
    errno=errno_save;
}