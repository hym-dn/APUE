#include<stdlib.h>
#include<pthread.h>

#define NHASH 29
#define HASH(fp) (((unsigned long)fp)%NHASH)
struct foo *fh[NHASH];

pthread_mutex_t hashlock=PTHREAD_MUTEX_INITIALIZER;

// 资源结构
struct foo{
    int f_count; // 引用计数
    pthread_mutex_t f_lock; // 互斥锁
    struct foo *f_next; /* protected by hashlock */
    int f_id;
    /* ... more stuff here ... */
};

// 分配一个资源结构
struct foo *foo_alloc(void){ /* allocate the object */
    struct foo *fp;
    int idx;
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
        // Hash计算
        idx=HASH(fp);
        // 上锁
        pthread_mutex_lock(&hashlock);
        // 设置下一个元素
        fp->f_next=fh[idx];
        // 存储当前结构
        fh[idx]=fp;
        // 上锁
        pthread_mutex_lock(&fp->f_lock);
        // 解锁
        pthread_mutex_unlock(&hashlock);
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
    fp->f_count++;
    // 解锁
    pthread_mutex_unlock(&fp->f_lock);
}

// 找到一个存在的对象
struct foo *foo_find(int id){ /* find an existing object */
    struct foo *fp;
    int idx;
    // 计算Hash位置
    idx=HASH(fp);
    // 上锁
    pthread_mutex_lock(&hashlock);
    // 查找指定元素
    for(fp=fh[idx];fp!=NULL;fp=fp->f_next){
        if(fp->f_id==id){
            foo_hold(fp);
            break;
        }
    }
    // 解锁
    pthread_mutex_unlock(&hashlock);
    // 返回
    return(fp);
}

// 释放资源
void foo_rele(struct foo *fp){ /* release a reference to the object */
    struct foo *tfp;
    int idx;
    // 上锁
    pthread_mutex_lock(&hashlock);
    // 引用计数自减
    if(--fp->f_count==0){ /* last reference */ // 计数为0
        // 删除Hash列表
        idx=HASH(fp);
        tfp=fh[idx];
        if(tfp==fp){
            fh[idx]=fp->f_next;
        }else{
            while(tfp->f_next!=fp){
                tfp=tfp->f_next;
            }
            tfp->f_next=fp->f_next;
        }
        // 解锁
        pthread_mutex_unlock(&hashlock);
        // 销毁
        pthread_mutex_destroy(&fp->f_lock);
        // 释放
        free(fp);
    }
    // 计数非0
    else{
        // 解锁
        pthread_mutex_unlock(&hashlock);
    }
}