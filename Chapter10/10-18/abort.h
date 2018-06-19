#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

// POSIX风格的abort()函数实现
void abort(void){ /* POSIX-style abort() function */
    // 屏蔽信号集
    sigset_t mask;
    // 信号信息结构体
    struct sigaction action;
    /**
     * Caller can't ignore SIGABRT，if so reset to default.
     */
    // 获取终止信号当前配置
    sigaction(SIGABRT,NULL,&action);
    // 如果当前忽略终止信号
    if(action.sa_handler==SIG_IGN){
        // 将终止信号设置为默认
        action.sa_handler=SIG_DFL;
        sigaction(SIGABRT,&action,NULL);
    }
    // 如果当前默认处理终止信号
    if(action.sa_handler==SIG_DFL){
        fflush(NULL); /* flush all open stdio streams */
    }
    /**
     * Caller can't block SIGABRT; make sure it's unblocked.
     */
    // 填充全部信号
    sigfillset(&mask);
    // 删除终止信号
    sigdelset(&mask,SIGABRT); /* mask has only SIGABRT turned off */
    // 屏蔽全部信号，除了SIGABRT
    sigprocmask(SIG_SETMASK,&mask,NULL);
    // 向自己发送终止信号
    kill(getpid(),SIGABRT); /* send the signal */
    /**
     * If we're here,process caught SIGABRT and returned.
     */
    // 冲洗全部IO流
    fflush(NULL); /* flush all open stdio streams */
    // 设置信号默认处理
    action.sa_handler=SIG_DFL;
    // 安装信号默认处理
    sigaction(SIGABRT,&action,NULL);
    // 屏蔽全部信号，除了SIGABRT
    sigprocmask(SIG_SETMASK,&mask,NULL); /* just in case ... */
    // 再次发送终止信号
    kill(getpid(),SIGABRT); /* and one more time */
    // 退出
    exit(1); /* this should never be executed ... */
} 