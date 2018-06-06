#include<setjmp.h>
#include<signal.h>
#include<unistd.h>

// 跳转标记
static jmp_buf env_alrm;

// 信号捕捉函数
static void sig_alrm(int signo){
    longjmp(env_alrm,1); // 跳转到指定位置
}

// 睡眠函数
unsigned int sleep2(unsigned int nsecs){
    // 信号安装
    if(signal(SIGALRM,sig_alrm)==SIG_ERR){ // 失败
        return(nsecs);
    }
    // 设置跳转标志
    if(setjmp(env_alrm)==0){
        // 设置定时器
        alarm(nsecs); /* start the timer */
        // 暂停等待信号唤醒
        pause(); /* next caught signal wakes us up*/
    }
    // 关闭定时器
    return(alarm(0)); /* turn off timer,return unslept time */
}