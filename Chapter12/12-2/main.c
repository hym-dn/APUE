#include"../../apue.h"
#include"../12-1/makethread.h"
#include<pthread.h>
#include<time.h>
#include<sys/time.h>

extern int makethread(void *(*)(void*),void*);

// 信息结构体
struct to_info{
    void (*to_fn)(void*); /* function */ // 函数
    void *to_arg; /* argument */ // 参数
    struct timespec to_wait; /* time to wait */ // 等待时间
};

#define SECTONSEC 1000000000 /* seconds to nanoseconds */
#define USECTONSEC 1000 /* microseconds to nanoseconds */

// 超时调用线程
void *timeout_helper(void *arg){
    struct to_info *tip;
    tip=(struct to_info *)arg;
    nanosleep(&tip->to_wait,NULL);
    (*tip->to_fn)(tip->to_arg);
    return(0);
}

// 超时函数
void timeout(const struct timespec *when,void (*func)(void *),void *arg){
    struct timespec now;
    struct timeval tv;
    struct to_info *tip;
    int err;
    // 获取当前时间
    gettimeofday(&tv,NULL);
    now.tv_sec=tv.tv_sec;
    now.tv_nsec=tv.tv_usec*USECTONSEC;
    if((when->tv_sec>now.tv_sec)||(when->tv_sec==now.tv_sec
        &&when->tv_nsec>now.tv_nsec)){ // 尚未超时，则开启超时等待
        // 分配信息结构
        tip=malloc(sizeof(struct to_info));
        // 分配成功
        if(tip!=NULL){
            // 设置调用函数
            tip->to_fn=func;
            // 设置调用参数
            tip->to_arg=arg;
            // 计算剩余时间
            tip->to_wait.tv_sec=when->tv_sec-now.tv_sec;
            if(when->tv_nsec>=now.tv_nsec){
                tip->to_wait.tv_nsec=when->tv_nsec-now.tv_nsec;
            }else{
                tip->to_wait.tv_sec--;
                tip->to_wait.tv_nsec=SECTONSEC-now.tv_nsec+when->tv_nsec;
            }
            // 启动线程
            err=makethread(timeout_helper,(void*)tip);
            if(0==err){
                return;
            }
        }
    }
    /**
     * We get here if (a) when<=now,or (b)malloc fails,or (c)
     * we can't make a thread,so we just call the function now. 
     */
    (*func)(arg);
}

pthread_mutexattr_t attr;
pthread_mutex_t mutex;

void retry(void *arg){
    pthread_mutex_lock(&mutex);
    /* perform retry steps ... */
    pthread_mutex_unlock(&mutex);
}

int main(void){
    int err,condition,arg;
    struct timespec when;
    // 初始化互斥量属性
    if((err=pthread_mutexattr_init(&attr))!=0){
        err_exit(err,"pthread_mutexattr_init failed");
    }
    // 设置互斥量递归类型
    if((err=pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE))!=0){
        err_exit(err,"can't set recursive type");
    }
    // 初始化互斥量
    if((err=pthread_mutex_init(&mutex,&attr))!=0){
        err_exit(err,"can't create recursive mutex");
    }
    /* ... */
    pthread_mutex_lock(&mutex);
    /* ... */
    if(condition){
        /* calculate target time "when" */
        timeout(&when,retry,(void*)arg);
    }
    /* ... */
    pthread_mutex_unlock(&mutex);
    /* ... */
    exit(0);
}