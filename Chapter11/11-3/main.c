#include"../../apue.h"
#include<pthread.h>

struct foo{
    int a,b,c,d;
};

// 打印foo结构内容
void printfoo(const char *s,const struct foo *fp){
    printf(s);
    // 打印结构地址
    printf(" structure at 0x%x\n",(unsigned)fp);
    // 打印结构内容
    printf(" foo.a=%d\n",fp->a);
    printf(" foo.b=%d\n",fp->b);
    printf(" foo.c=%d\n",fp->c);
    printf(" foo.d=%d\n",fp->d);
}

// 线程函数1
void *thr_fn1(void *arg){
    struct foo foo={1,2,3,4};
    printfoo("thread 1:\n",&foo);
    pthread_exit((void*)&foo);
}

// 线程函数2
void *thr_fn2(void *arg){
    printf("thread 2: ID is %d\n",pthread_self());
    pthread_exit((void*)0);
}

int main(void){
    int err;
    pthread_t tid1,tid2;
    struct foo *fp;
    // 创建线程1
    err=pthread_create(&tid1,NULL,thr_fn1,NULL);
    if(0!=err){
        err_quit("can't create thread 1: %s\n",strerror(err));
    }
    // 等待线程1退出
    err=pthread_join(tid1,(void*)&fp);
    if(0!=err){
        err_quit("can't join with thread 1: %s\n",strerror(err));
    }
    // 等待1秒
    sleep(1);
    // 输出信息
    printf("parent starting second thread\n");
    // 创建线程2
    err=pthread_create(&tid2,NULL,thr_fn2,NULL);
    if(0!=err){
        err_quit("can't create thread 2: %s\n",strerror(err));
    }
    // 等待1秒
    sleep(1);
    // 打印信息
    printfoo("parent:\n",fp);
    // 退出
    exit(0);
}