#include"../../apue.h"

// 闹钟信号捕捉函数
static void sig_alrm(int signo){
    /* nothing to do,just returning wakes up sigsuspend() */
}

// sleep实现
unsigned int sleep(unsigned int nsecs){
    struct sigaction newact,oldact;
    sigset_t newmask,oldmask,suspmask;
    unsigned int unslept;
    /* set our handler,save previous information */
    // 设置闹钟信号处理流程
    newact.sa_handler=sig_alrm; // 设置处理函数
    sigemptyset(&newact.sa_mask); // 清空信号屏蔽字
    newact.sa_flags=0; // 清空标志
    sigaction(SIGALRM,&newact,&oldact); // 设置信号处理，并且保存之前的处理
    /* block SIGALRM and save current signal mask */
    // 自动闹钟信号，并且保存当前信号屏蔽字
    sigemptyset(&newmask); // 清空信号集
    sigaddset(&newmask,SIGALRM); // 设置屏蔽字
    sigprocmask(SIG_BLOCK,&newmask,&oldmask); // 设置信号屏蔽字
    // 启动闹钟
    alarm(nsecs);
    // 记录以前屏蔽字
    suspmask=oldmask;
    // 删除闹钟屏蔽字
    sigdelset(&suspmask,SIGALRM); /* make sure SIGALRM isn't blocked */
    // 挂起，等待信号触发
    sigsuspend(&suspmask); /* wait for any signal to be caught */
    // 一些信号已经被捕获，此时SIGALRAM信号处于阻塞状态
    /* some signal has been caught,SIGALRM is now blocked */
    // 获取闹钟剩余时间
    unslept=alarm(0);
    // 恢复闹钟信号处理过程
    sigaction(SIGALRM,&oldact,NULL); /* reset previous action */
    // 恢复信号屏蔽字
    /* reset signal mask,which unblocks SIGALRM */
    sigprocmask(SIG_SETMASK,&oldmask,NULL);
    // 返回
    return(unslept);
}