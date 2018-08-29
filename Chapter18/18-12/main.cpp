#include"../../apue.h"
#include<termios.h>
#ifndef TIOCGWINSZ
#include<sys/ioctl.h>
#endif

// 读取窗口尺寸，并且输出
static void pr_winsize(int fd){
    struct winsize size;
    if(ioctl(fd,TIOCGWINSZ,(char*)&size)<0){
        err_sys("TIOCGWINSZ error");
    }
    printf("%d rows,%d columns\n",size.ws_row,size.ws_col);
}

// 窗口尺寸变更信号捕获函数
static void sig_winch(int /*signo*/){
    printf("SIGWINCH received\n");
    pr_winsize(STDIN_FILENO);
}

// 入口函数
int main(void){
    // 标准输入为终端设备
    if(isatty(STDIN_FILENO)==0){
        exit(1);
    }
    // 绑定信号
    if(signal(SIGWINCH,sig_winch)==SIG_ERR){
        err_sys("signal error");
    }
    // 输出尺寸
    pr_winsize(STDIN_FILENO);
    // 等待窗口尺寸变更信号触发
    for(;;){
        pause();
    }
}