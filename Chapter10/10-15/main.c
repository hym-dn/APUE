#include"../10-12/signal.h"
#include"../10-10/pr_mask.h"

static void sig_int(int);

int main(void){
    // 信号集合
    sigset_t newmask,oldmask,waitmask;
    // 打印进程信号屏蔽字
    pr_mask("program start: ");
    // 安装信号处理函数
    if(signal(SIGINT,sig_int)==SIG_ERR){
        err_sys("signal(SIGINT) error");
    }
    // 设置信号集
    sigemptyset(&waitmask);
    sigaddset(&waitmask,SIGUSR1);
    sigemptyset(&newmask);
    sigaddset(&newmask,SIGINT);
    // 阻塞SIGINT信号，并且存储当前的信号屏蔽字
    /**
     *  Block SIGINT and save current signal mask.
     */
    if(sigprocmask(SIG_BLOCK,&newmask,&oldmask)<0){
        err_sys("SIG_BLOCK error");
    }
    // 关键代码段
    /**
     * Critical region of code.
     */
    // 打印信号屏蔽字
    pr_mask("in critical region: ");
    // 暂停，阻塞全部的信号，除了SIGUSR1
    /**
     * Pause,allowing all signals except SIGUSR1.
     */
    if(sigsuspend(&waitmask)!=-1){
        err_sys("sigsuspend error");
    }
    // 打印信号屏蔽字
    pr_mask("after return from sigsuspend: ");
    // 重置信号屏蔽字，取消SIGINT信号阻塞
    /**
     * Reset signal mask which unblocks SIGINT. 
     */
    if(sigprocmask(SIG_SETMASK,&oldmask,NULL)<0){
        err_sys("SIG_SETMASK error");
    }
    /**
     * And continue processing ... 
     */
    // 打印信号屏蔽字
    pr_mask("program exit: ");
    // 退出
    exit(0);
}

// 信号捕获函数
static void sig_int(int signo){
    pr_mask("\nin sig_int: ");
}