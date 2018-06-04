#include"../../apue.h"
#include<sys/wait.h>

static void sig_cld(int);

int main(){
    // 进程ID
    pid_t pid;
    // 安装信号处理函数
    if(signal(SIGCLD,sig_cld)==SIG_ERR){ // 失败
        perror("signal error");
    }
    // 创建子进程
    if((pid=fork())<0){ // 创建失败
        perror("fork error");
    }
    // 子进程
    else{ /* child */
        sleep(2);
        _exit(0);
    }
    // 暂停等待信号触发
    pause(); /* parent */
    // 退出
    exit(0);
}

static void sig_cld(int signo){ /* interrupts pause() */
    // 进程ID、状态
    pid_t pid;
    int status;
    // 打印信息
    printf("SIGCLD received\n");
    // 重新安装信号处理函数
    if(signal(SIGCLD,sig_cld)==SIG_ERR){ /* reestablish handler */
        perror("signal error");
    }
    // 等待子进程退出
    if((pid=wait(&status))<0){ /* fetch child status */
        perror("wait error");
    }
    // 打印信息
    printf("pid=%d\n",pid);
}