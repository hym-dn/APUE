#include"../../apue.h"

static void sig_alrm(int);

int main(void){
    int n;
    char line[MAXLINE];
    // 安装信号
    if(signal(SIGALRM,sig_alrm)==SIG_ERR){
        err_sys("signal(SIGALRM) error");
    }
    // 开始定时器
    alarm(10);
    // 从标准输入读取一行
    if((n=read(STDIN_FILENO,line,MAXLINE))<0){
        err_sys("read error");
    }
    // 关闭定时器
    alarm(0);
    // 向标准输出写一行
    write(STDOUT_FILENO,line,n);
    // 退出
    exit(0);
}

static void sig_alrm(int signo){
    /* nothing to do,just return to interrupt the read */
}