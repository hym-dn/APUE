#include"../../apue.h"
#include<pthread.h>

// 第1个线程函数
void *thr_fn1(void *arg){
    // 打印线程信息
    printf("thread 1 returning\n");
    // 返回退出码
    return((void*)1);
}

// 第2个线程函数
void *thr_fn2(void *arg){
    // 打印线程信息
    printf("thread 2 exiting\n");
    // 返回退出码
    pthread_exit((void*)2);
}

int main(void){
    int err;
    pthread_t tid1,tid2;
    void *tret;
    // 创建线程1
    err=pthread_create(&tid1,NULL,thr_fn1,NULL);
    if(0!=err){
        err_quit("can't create thread 1: %s\n",strerror(err));
    }
    // 创建线程2
    err=pthread_create(&tid2,NULL,thr_fn2,NULL);
    if(0!=err){
        err_quit("can't create thread 2: %s\n",strerror(err));
    }
    // 等待线程1退出
    err=pthread_join(tid1,&tret);
    if(0!=err){
        err_quit("can't join with thread 1: %s\n",strerror(err));
    }
    printf("thread 1 exit code %d\n",(int)tret);
    // 等待线程2退出
    err=pthread_join(tid2,&tret);
    if(0!=err){
        err_quit("can't join with thread 2: %s\n",strerror(err));
    }
    printf("thread 2 exit code %d\n",(int)tret);
    // 退出
    exit(0);
}