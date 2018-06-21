#include"../../apue.h"
#include<pthread.h>

// 线程清理函数
void cleanup(void *arg){
    printf("cleanup: %s\n",(char*)arg);
}

// 线程函数1
void *thr_fn1(void *arg){
    // 打印线程信息
    printf("thread 1 start\n");
    // 安装线程清理函数
    pthread_cleanup_push(cleanup,"thread 1 first handler");
    pthread_cleanup_push(cleanup,"thread 1 second handler");
    // 如果参数非空
    if(arg){
        //return((void*)1);
        pthread_exit((void*)1);
    }
    // 卸载线程清理函数
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
    // 退出线程
    pthread_exit((void*)1);
}

// 线程函数2
void *thr_fn2(void *arg){
    // 打印线程信息
    printf("thread 2 start\n");
    // 安装线程清理函数
    pthread_cleanup_push(cleanup,"thread 2 first handler");
    pthread_cleanup_push(cleanup,"thread 2 second handler");
    // 如果参数非空
    if(arg){
        //return((void*)2);
        pthread_exit((void*)2);
    }
    // 卸载线程清理函数
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
    // 退出线程
    pthread_exit((void*)2);
}

int main(void){
    int err;
    pthread_t tid1,tid2;
    void *tret;
    // 创建线程1
    err=pthread_create(&tid1,NULL,thr_fn1,(void*)1);
    if(0!=err){
        err_quit("can't create thread 1: %s\n",strerror(err));
    }
    // 创建线程2
    err=pthread_create(&tid2,NULL,thr_fn2,(void*)2);
    if(0!=err){
        err_quit("can't create thread 2: %s\n",strerror(err));
    }
    // 等待线程1退出
    err=pthread_join(tid1,&tret);
    if(err!=0){
        err_quit("can't join with thread 1: %s\n",strerror(err));
    }
    printf("thread 1 exit code %d\n",(int)tret);
    // 等待线程2退出
    err=pthread_join(tid2,&tret);
    if(err!=0){
        err_quit("can't join with thread 2: %s\n",strerror(err));
    }
    printf("thread 2 exit code %d\n",(int)tret);
    // 退出
    exit(0);
}