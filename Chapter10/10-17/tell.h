#include"../10-12/signal.h"

static volatile sig_atomic_t sigflag; /* set nonzero by sig handler */
static sigset_t newmask,oldmask,zeromask;

// 信号捕捉函数
static void sig_usr(int signo){ /* one signal handler for SIGUSR1 and SIGUSR2 */
    sigflag=1;
}

void TELL_WAIT(void){
    // 安装信号
    if(signal(SIGUSR1,sig_usr)==SIG_ERR){
        err_sys("signal(SIGUSR1) error");
    }
    if(signal(SIGUSR2,sig_usr)==SIG_ERR){
        err_sys("signal(SIGUSR2) error");
    }
    // 设置信号集
    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigaddset(&newmask,SIGUSR1);
    sigaddset(&newmask,SIGUSR2);
    // 阻塞SIGUSR1、SIGUSR2，并且存储当前信号屏蔽字
    /**
     * Block SIGUSR1 and SIGUSR2,and save current signal mask.
     */
    if(sigprocmask(SIG_BLOCK,&newmask,&oldmask)<0){
        err_sys("SIG_BLOCK error");
    }
}

// 通知父亲进程
void TELL_PARENT(pid_t pid){
    kill(pid,SIGUSR2); /* tell parent we're done */
}

// 等待父进程
void WAIT_PARENT(void){
    // 等待父进程信号
    while(sigflag==0){
        sigsuspend(&zeromask); /* and wait for parent */
    }
    // 重置信号标志
    sigflag=0;
    // 恢复老的信号屏蔽字
    /**
     * Reset signal mask to original value.
     */
    if(sigprocmask(SIG_SETMASK,&oldmask,NULL)<0){
        err_sys("SIG_SETMASK error");
    }
}

// 通知子进程
void TELL_CHILD(pid_t pid){
    kill(pid,SIGUSR1);
}

// 等待字进程
void WAIT_CHILD(void){
    // 等待子进程信号
    while(sigflag==0){
        sigsuspend(&zeromask); /* and wait for child */
    }
    // 重置信号标志
    sigflag=0;
    // 恢复老的信号屏蔽字
    /**
     * Reset signal mask to original value.
     */
    if(sigprocmask(SIG_SETMASK,&oldmask,NULL)<0){
        err_sys("SIG_SETMASK error");
    }
}