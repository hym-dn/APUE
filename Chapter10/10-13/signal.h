#include"../../apue.h"

// 利用sigaction重新实现signal函数
/* Reliable version of signal(),using POSIX sigaction(). */
Sigfunc *signal(int signo,Sigfunc *func){
    struct sigaction act,oact;
    // 设置信号捕捉函数
    act.sa_handler=func;
    // 清空屏蔽信号字
    sigemptyset(&act.sa_mask);
    // 清空信号信号处理选项
    act.sa_flags=0;
    // 禁止系统调用自动重启
#ifdef SA_INTERRUPT
    act.sa_flags|=SA_INTERRUPT;
#endif
    // 捕获信号
    if(sigaction(signo,&act,&oact)<0){
        return(SIG_ERR);
    }
    // 返回老的捕获函数
    return(oact.sa_handler);
}