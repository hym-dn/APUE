#include"../../apue.h"
#include"../10-10/pr_mask.h"
#include<setjmp.h>
#include<time.h>

static void sig_usr1(int);
static void sig_alrm(int);
static sigjmp_buf jmpbuf;
static volatile sig_atomic_t canjump;

int main(void){
    // 安装信号捕获函数
    if(signal(SIGUSR1,sig_usr1)==SIG_ERR){
        err_sys("signal(SIGUSR1) error");
    }
    if(signal(SIGALRM,sig_alrm)==SIG_ERR){
        err_sys("signal(SIGALRM) error");
    }
    // 打印信号屏蔽字
    pr_mask("starting main: ");
    // 设置跳转标志
    if(sigsetjmp(jmpbuf,1)){
        // 打印信号屏蔽字
        pr_mask("ending main: ");
        // 退出
        exit(0);
    }
    // 启用跳转标志
    canjump=1; /* now sigsetjmp() is OK */
    // 等待信号触发
    for(;;){
        pause();
    }
}

// SIGUSR1信号捕获函数
static void sig_usr1(int signo){
    time_t starttime;
    // 如果跳转标志尚未设置
    if(canjump==0){
        return; /* unexpected signal,ignore */
    }
    // 打印屏蔽字
    pr_mask("starting sig_usr1: ");
    // 安装闹钟
    alarm(3); /* SIGALRM in 3 seconds */
    // 记录启动时间
    starttime=time(NULL);
    // 等待5秒钟
    for(;;){
        if(time(NULL)>starttime+5){
            break;
        }
    }
    // 打印屏蔽字
    pr_mask("finishing sig_usr1: ");
    // 清除跳转标志
    canjump=0;
    // 跳转
    siglongjmp(jmpbuf,1); /* jump back to main,don't return */
}

// SIGALRM信号捕获函数
static void sig_alrm(int signo){
    pr_mask("in sig_alrm: ");
}