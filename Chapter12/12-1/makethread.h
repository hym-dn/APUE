#include"../../apue.h"
#include<pthread.h>

int makethread(void *(*fn)(void *),void *arg){
    int err; // 错误编号
    pthread_t tid; // 线程ID
    pthread_attr_t attr; // 线程属性
    // 初始化线程属性
    err=pthread_attr_init(&attr);
    if(err!=0){
        return(err);
    }
    // 设置线程的分离状态
    err=pthread_attr_setdetachstate(
        &attr,PTHREAD_CREATE_DETACHED);
    // 创建分离状态线程
    if(err==0){
        err=pthread_create(&tid,&attr,fn,arg);
    }
    // 销毁属性
    pthread_attr_destroy(&attr);
    // 返回
    return(err);
}