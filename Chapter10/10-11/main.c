#include"../../apue.h"

static void sig_quit(int);

int main(void){
    // 定义信号集
    sigset_t newmask,oldmask,pendmask;
    // 安装信号集
    if(signal(SIGQUIT,sig_quit)==SIG_ERR){
        err_sys("can't catch SIGQUIT");
    }
    // 阻塞退出并且保存当前信号屏蔽字
    /**
     * Block SIGQUIT and save current signal mask.
     */
    // 清空字符集
    sigemptyset(&newmask);
    // 增加退出信号
    sigaddset(&newmask,SIGQUIT);
    // 阻塞SIG_QUIT信号
    if(sigprocmask(SIG_BLOCK,&newmask,&oldmask)<0){
        err_sys("SIG_BLOCK error");
    }
    // 退出信号挂起
    sleep(5); /* SIGQUIT here will remain pending */
    // 获取未决信号
    if(sigpending(&pendmask)<0){
        err_sys("sigpending error");
    }
    // 判断退出信号是否未决
    if(sigismember(&pendmask,SIGQUIT)){
        printf("\nSIGQUIT pending\n");
    }
    // 取消退出信号阻塞
    /**
     *  Reset signal mask which unblocks SIGQUIT.
     */
    if(sigprocmask(SIG_SETMASK,&oldmask,NULL)<0){
        err_sys("SIG_SETMASK error");
    }
    // 输出信息
    printf("SIGQUIT unblocked\n");
    // 等待信号响应
    sleep(5); /* SIGQUIT here will terminate with core file */
    // 退出
    exit(0);
}

// 退出信号捕获函数
static void sig_quit(int signo){
    // 打印相关信息
    printf("caught SIGQUIT\n");
    // 设置退出信号为默认处理
    if(signal(SIGQUIT,SIG_DFL)==SIG_ERR){
        err_sys("can't reset SIGQUIT");
    }
}