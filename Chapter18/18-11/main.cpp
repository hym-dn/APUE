#include"../../apue.h"
#include"../18-10/termiomode.h"

// 信号捕捉函数
static void sig_catch(int /*signo*/){
    // 输出信息
    printf("signal caught\n");
    // 回复终端状态
    tty_reset(STDIN_FILENO);
    // 结束进程
    exit(0);
}

int main(void){
    int i;
    char c;
    // 捕获相关信号
    if(signal(SIGINT,sig_catch)==SIG_ERR){ // 中断 /* catch signals */
        err_sys("signal(SIGINT) error");
    }
    if(signal(SIGQUIT,sig_catch)==SIG_ERR){ // 退出
        err_sys("signal(SIGQUIT) error");
    }
    if(signal(SIGTERM,sig_catch)==SIG_ERR){ // 终止
        err_sys("signal(SIGTERM) error");
    }
    // 命令标准输入进入原始模式
    if(tty_raw(STDIN_FILENO)<0){
        err_sys("tty_raw error");
    }
    // 输出提示信息
    printf("Enter raw mode characters,terminate with DELETE\n");
    // 从标准输入读，直到读取DELETE未知
    while((i=read(STDIN_FILENO,&c,1))==1){
        if((c&=255)==0177){ /* 0177=ASCII DELETE */
            break;
        }
        printf("%o\n",c);
    }
    // 恢复终端状态
    if(tty_reset(STDIN_FILENO)<0){
        err_sys("tty_reset error");
    }
    // 读终端错误处理
    if(i<=0){
        err_sys("read error");
    }
    // 命令标准输入进入BREAK模式
    if(tty_cbreak(STDIN_FILENO)<0){
        err_sys("tty_cbreak error");
    }
    // 输出提示信息
    printf("Enter raw mode characters,terminate with SIGINT\n");
    // 从标准输入读
    while((i=read(STDIN_FILENO,&c,1))==1){
        c&=255;
        printf("%o\n",c);
    }
    // 恢复终端状态
    if(tty_reset(STDIN_FILENO)<0){
        err_sys("tty_reset error");
    }
    // 读错误处理
    if(i<=0){
        err_sys("read error");
    }
    // 退出进程
    exit(0);
}