#include"../10-12/signal.h"

#define BUFFSIZE 1024

static void sig_tstp(int);

int main(void){
    int n;
    char buf[BUFFSIZE];
    /**
     * Only catch SIGTSTP if we're running with a job-control shell. 
     */
    // 如果当前进程中，SIGTSTP信号是默认处理流程
    if(signal(SIGTSTP,SIG_IGN)==SIG_DFL){
        signal(SIGTSTP,sig_tstp); // 安装STGTSTP信号处理
    }
    // 从标准输入中读信息
    while((n=read(STDIN_FILENO,buf,BUFFSIZE))>0){
        // 向标准输出中写信息
        if(write(STDOUT_FILENO,buf,n)!=n){
            err_sys("write error");
        }
    }
    // 如果读、写终端失败
    if(n<0){
        err_sys("read error");
    }
    // 退出
    exit(0);
}

// STGTSTP信号捕捉函数
static void sig_tstp(int signo){ /* signal handler for SIGTSTP */
    sigset_t mask;
    /* ... move cursor to lower left corner,reset tty mode ... */
    /**
     * Unblock SIGTSTP,since it's blocked while we're handling it. 
     */
    // 取消对SIGTSTP的阻塞
    sigemptyset(&mask);
    sigaddset(&mask,SIGTSTP);
    sigprocmask(SIG_UNBLOCK,&mask,NULL);
    // 重置处理为默认
    signal(SIGTSTP,SIG_DFL); /* reset disposition to default */
    // 给自己发送信号
    kill(getpid(),SIGTSTP); /* and send the signal to ourself */
    // 我们将不从kill返回，知道收到SIGCONT信号
    /* we won't return from the kill until we're continued */
    signal(SIGTSTP,sig_tstp); /* reestablish signal handler */
    /* ... reset tty mode,redraw screen ... */
}