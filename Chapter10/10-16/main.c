#include"../10-12/signal.h"

// 标志变量
volatile sig_atomic_t quitflag; /* set nonzero by signal handler */

// 信号捕获函数
static void sig_int(int signo){ /* one signal handler for SIGINT and SIGQUIT */
    // 中断信号
    if(signo==SIGINT){
        printf("\ninterrupt\n");
    }
    // 退出信号
    else if(signo==SIGQUIT){
        quitflag=1; /* set flag for main loop */
    }
}

int main(void){
    // 信号集
    sigset_t newmask,oldmask,zeromask;
    // 安装信号
    if(signal(SIGINT,sig_int)==SIG_ERR){
        err_sys("signal(SIGINT) error");
    }
    if(signal(SIGQUIT,sig_int)==SIG_ERR){
        err_sys("signal(SIGQUIT) error");
    }
    // 设置信号集
    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigaddset(&newmask,SIGQUIT);
    // 阻塞退出信号，并且保存当前信号屏蔽字
    /**
     * Block SIGQUIT and save current signal mask.
     */
    if(sigprocmask(SIG_BLOCK,&newmask,&oldmask)<0){
        err_sys("SIG_BLOCK error");
    }
    // 等待退出信号产生
    while(quitflag==0){
        sigsuspend(&zeromask);
    }
    /**
     * SIGQUIT has been caught and is now blocked; do whatever.
     */
    // 重置退出信号标志
    quitflag=0;
    // 恢复信号屏蔽字
    /**
     * Reset signal mask which unblocks SIGQUIT.
     */
    if(sigprocmask(SIG_SETMASK,&oldmask,NULL)<0){
        err_sys("SIG_SETMASK error");
    }
    // 退出
    exit(0);
}