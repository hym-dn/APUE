#include<signal.h>
#include<unistd.h>

// 信号捕捉函数
static void sig_alrm(int signo){
    /* nothing to do,just return to wake up the pause */
}

unsigned int sleep1(unsigned int nsecs){
    // 安装信号
    if(signal(SIGALRM,sig_alrm)==SIG_ERR){ // 安装失败
        return(nsecs);
    }
    // 闹钟
    alarm(nsecs); /* start the timer */
    // 暂停,等待信号
    pause(); /* next caught signal wakes us up */
    // 停止闹钟
    return(alarm(0)); /* turn off timer,return unslept time */
}