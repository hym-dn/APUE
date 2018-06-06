#include"../../apue.h"

unsigned int sleep2(unsigned int);
static void sig_int(int);

int main(void){
    unsigned int unslept;
    // 安装信号
    if(signal(SIGINT,sig_int)==SIG_ERR){ // 失败
        err_sys("signal(SIGINT) error");
    }
    // 休眠
    unslept=sleep2(5);
    // 打印消息
    printf("sleep2 returned: %u\n",unslept);
    // 退出
    exit(0);
}

// 信号捕捉函数
static void sig_int(int signo){
    int i,j;
    volatile int k;
    /**
     * 拖延超过5秒钟的时间
     * Tune these loops to run for more than 5 seconds
     * on whaterver system this test program is run.
     */
    // 打印信息
    printf("\nsig_int starting\n");
    // 拖延时间
    for(i=0;i<300000;i++){
        for(j=0;j<4000;j++){
            k+=i*j;
        }
    }
    // 打印信息
    printf("sig_int finished\n");
}