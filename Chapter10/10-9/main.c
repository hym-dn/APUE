#include<signal.h>
#include<errno.h>

// 检测信号是否合法
/* <signal.h> usually define NSIG to include signal number 0 */
#define SIGBAD(signo) ((signo)<=0||(signo)>=NSIG)

// 添加信号
int sigaddset(sigset_t *set,int signo){
    // 如果信号非法
    if(SIGBAD(signo)){
        // 设置错误
        errno=EINVAL;
        // 返回错误
        return(-1);
    }
    // 设置信号
    *set|=1<<(signo-1); /* turn bit on */
    // 成功返回
    return(0);
}

// 删除信号
int sigdelset(sigset_t *set,int signo){
    // 如果信号非法
    if(SIGBAD(signo)){
        // 设置错误
        errno=EINVAL;
        // 返回错误
        return(-1);
    }
    // 取消信号
    *set&=~(1<<(signo-1)); /* turn bit off */
    // 成功返回
    return(0);
}

// 检测信号是否为集合成员
int sigismember(const sigset_t *set,int signo){
    // 如果信号非法
    if(SIGBAD(signo)){
        // 设置错误
        errno=EINVAL;
        // 返回错误
        return(-1);
    }
    // 返回检测结果
    return((*set&(1<<(signo-1)))!=0);
}