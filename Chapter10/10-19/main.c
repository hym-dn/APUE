#include"../10-12/signal.h"
#include"../../Chapter8/8-12/main.c"

// SIGINT 信号捕捉函数
static void sig_int(int signo){
    printf("caught SIGINT\n");
}

// SIGCHLD 信号捕捉函数
static void sig_chld(int signo){
    printf("caught SIGCHLD\n");
}

int main(void){
    // 安装信号
    if(signal(SIGINT,sig_int)==SIG_ERR){
        err_sys("signal(SIGINT) error");
    }
    if(signal(SIGCHLD,sig_chld)==SIG_ERR){
        err_sys("signal(SIGCHLD) error");
    }
    // 执行ed编辑器
    if(system("/bin/ed")<0){
        err_sys("system() error");
    }
    // 退出
    exit(0);
}
