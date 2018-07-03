#include"../../apue.h"
#include<pthread.h>

int quitflag; /* setnozero by thread*/
sigset_t mask;

pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER; // 互斥量
pthread_cond_t wait=PTHREAD_COND_INITIALIZER; // 条件量

// 线程函数
void *thr_fn(void *arg){
    int err,signo;
    for(;;){
        // 等待信号产生
        err=sigwait(&mask,&signo);
        if(0!=err){
            err_exit(err,"sigwait failed");
        }
        // 信号分类处理
        switch(signo){
        // 中断信号
        case SIGINT:
            printf("\ninterrupt\n");
            break;
        // 退出信号
        case SIGQUIT:
            printf("\nquit\n");
            // 上锁
            pthread_mutex_lock(&lock);
            // 标志
            quitflag=1;
            // 解锁
            pthread_mutex_unlock(&lock);
            // 条件达成
            pthread_cond_signal(&wait);
            // 返回
            return(0);
        // 默认
        default:
            printf("unexpected signal %d\n",signo);
            exit(1);
        }
    }
}

// 入口函数
int main(void){
    int err;
    sigset_t oldmask;
    pthread_t tid;
    // 设置信号集
    // 清空
    sigemptyset(&mask);
    // 添加
    sigaddset(&mask,SIGINT);
    sigaddset(&mask,SIGQUIT);
    // 设置信号屏蔽字
    if((err=pthread_sigmask(SIG_BLOCK,&mask,&oldmask))!=0){
        err_exit(err,"SIG_BLOCK error");
    }
    // 创建线程
    err=pthread_create(&tid,NULL,thr_fn,0);
    if(err!=0){
        err_exit(err,"can't create thread");
    }
    // 加锁
    pthread_mutex_lock(&lock);
    // 等待条件达成
    while(quitflag==0){
        pthread_cond_wait(&wait,&lock);
    }
    // 解锁
    pthread_mutex_unlock(&lock);
    // 重置标记
    /* SIGQUIT has been caught and is now blocked; do whatever */
    quitflag=0;
    // 重置信号屏蔽字
    /* reset signal mask which unblocks SIGQUIT */
    if(sigprocmask(SIG_SETMASK,&oldmask,NULL)<0){
        err_sys("SIG_SETMASK error");
    }
    // 退出
    exit(0);
}