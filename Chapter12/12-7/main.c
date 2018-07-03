#include"../../apue.h"
#include<pthread.h>

// 初始化互斥量
pthread_mutex_t lock1=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2=PTHREAD_MUTEX_INITIALIZER;

// 父进程统计、加锁
void prepare(void){
    printf("preparing locks ... \n");
    pthread_mutex_lock(&lock1);
    pthread_mutex_lock(&lock2);
}

// 父进程解锁
void parent(void){
    printf("parent unlocking locks ... \n");
    pthread_mutex_unlock(&lock1);
    pthread_mutex_unlock(&lock2);
}

// 子进程解锁
void child(void){
    printf("child unlocking locks ... \n");
    pthread_mutex_unlock(&lock1);
    pthread_mutex_unlock(&lock2);
}

// 线程函数
void *thr_fn(void *arg){
    printf("thread started ... \n");
    pause();
    return(0);
}

// 入口程序
int main(void){
    int err;
    pid_t pid;
    pthread_t tid;
    // 如果是BSD,MACOS
#if defined(BSD)||defined(MACOS)
    printf("pthread_atfork is unsupported\n");
#else
    // 设置解锁函数
    if((err=pthread_atfork(prepare,parent,child))!=0){
        err_exit(err,"can't install fork handlers");
    }
    // 创建线程
    err=pthread_create(&tid,NULL,thr_fn,0);
    if(0!=err){
        err_exit(err,"can't create thread");
    }
    // 睡眠
    sleep(2);
    // 打印信息
    printf("parent about to fork ...\n");
    // 创建子进程
    if((pid=fork())<0){
        err_quit("fork failed");
    }else if(pid==0){ /* child */
        printf("child returned from fork\n");
    }else{ /* parent */
        printf("parent returned from fork\n");
    }
#endif
    // 退出
    exit(0);
}