#include"../../apue.h"
#include<pthread.h>

pthread_t ntid;

// 打印线程、进程ID
void printids(const char *s){
    pid_t pid;
    pthread_t tid;
    // 获取进程ID
    pid=getpid();
    // 获取线程ID
    tid=pthread_self();
    // 打印进程、线程ID
    printf("%s pid %u tid %u (0x%x)\n",s,
        (unsigned int)pid,(unsigned int)tid,
        (unsigned int )tid);
}

// 线程函数
void *thr_fn(void *arg){
    printids("new thread: ");
    return((void*)0);
}

int main(void){
    int err;
    // 创建线程
    err=pthread_create(&ntid,NULL,thr_fn,NULL);
    // 创建失败
    if(0!=err){
        err_quit("can't create thread: %s\n",
            strerror(err));
    }
    // 打印ID
    printids("main thread:");
    // 休眠
    sleep(1);
    // 退出
    exit(0);
}