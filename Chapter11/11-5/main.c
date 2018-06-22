#include<stdlib.h>
#include<pthread.h>

// 资源结构
struct foo{
    int f_count; // 引用计数
    pthread_mutex_t f_lock; // 互斥锁
    /* ... more stuff here ... */
};

// 分配一个资源结构
struct foo *foo_alloc(void){ /* allocate the object */
    struct foo *fp;
    // 分配资源结构
    if((fp=malloc(sizeof(struct foo)))!=NULL){ // 分配成功
        // 初始化引用计数
        fp->f_count=1;
        // 初始化互斥锁
        if(pthread_mutex_init(&fp->f_lock,NULL)!=0){ // 失败
            // 释放互斥锁
            free(fp); 
            // 返回空
            return(NULL);
        }
        /* ... continue initialization ... */
    }
    // 返回
    return(fp);
}

// 为资源增加一个引用计数
void foo_hold(struct foo *fp){ /* add a reference to the object */
    // 加锁
    pthread_mutex_lock(&fp->f_lock);
    // 计数自增
    fp->f_count;
    // 解锁
    pthread_mutex_unlock(&fp->f_lock);
}

// 释放资源
void foo_rele(struct foo *fp){ /* release a reference to the object */
    // 上锁
    pthread_mutex_lock(&fp->f_lock);
    // 引用计数自减
    if(--fp->f_count==0){ /* last reference */ // 计数为0
        // 解锁
        pthread_mutex_unlock(&fp->f_lock);
        // 销毁
        pthread_mutex_destroy(&fp->f_lock);
        // 释放
        free(fp);
    }
    // 计数非0
    else{
        // 解锁
        pthread_mutex_unlock(&fp->f_lock);
    }
}