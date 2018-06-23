#include<stdlib.h>
#include<pthread.h>

// 作业信息结构体
struct job{
    struct job *j_next;
    struct job *j_prev;
    pthread_t j_id; /* tells which thread handles this job */
    /* ... more stuff here ... */
};

// 作业队列结构体
struct queue{
    struct job *q_head;
    struct job *q_tail;
    pthread_rwlock_t q_lock;
};

/**
 * Initialize a queue 
 */
// 初始化队列
int queue_init(struct queue *qp){
    int err;
    // 初始化队列首、尾
    qp->q_head=NULL;
    qp->q_tail=NULL;
    // 初始化队列锁
    err=pthread_rwlock_init(&qp->q_lock,NULL);
    if(err!=0){
        return(err);
    }
    /* ... continue initialization ... */
    // 将继续其他的初始化
    return(0);
}

/**
 * Insert a job at the head of the queue. 
 */
// 在队首插入一个作业
void job_insert(struct queue *qp,struct job *jp){
    // 加写锁
    pthread_rwlock_wrlock(&qp->q_lock);
    // 插入作业
    jp->j_next=qp->q_head;
    jp->j_prev=NULL;
    if(qp->q_head!=NULL){
        qp->q_head->j_prev=jp;
    }else{
        qp->q_tail=jp; /* list was empty */
    }
    qp->q_head=jp;
    // 解锁
    pthread_rwlock_unlock(&qp->q_lock);
}

/**
 * Append a job on the tail of the queue.
 */
// 在对尾插入一个作业
void job_append(struct queue *qp,struct job *jp){
    // 加写锁
    pthread_rwlock_wrlock(&qp->q_lock);
    // 插入作业
    jp->j_next=NULL;
    jp->j_prev=qp->q_tail;
    if(qp->q_tail!=NULL){
        qp->q_tail->j_next=jp;
    }else{
        qp->q_head=jp; /* list was empty */
    }
    qp->q_tail=jp;
    // 解锁
    pthread_rwlock_unlock(&qp->q_lock);
}

/**
 * Remove the given job from a queue. 
 */
// 从队列中删除给定的作业
void job_remove(struct queue *qp,struct job *jp){
    // 加写锁
    pthread_rwlock_wrlock(&qp->q_lock);
    // 删除作业
    if(jp==qp->q_head){
        qp->q_head=jp->j_next;
        if(qp->q_tail==jp){
            qp->q_tail=NULL;
        }
    }else if(jp==qp->q_tail){
        qp->q_tail=jp->j_prev;
        if(qp->q_head==jp){
            qp->q_head=NULL;
        }
    }else{
        jp->j_prev->j_next=jp->j_next;
        jp->j_next->j_prev=jp->j_prev;
    }
    // 解锁
    pthread_rwlock_unlock(&qp->q_lock);
}

/**
 * Find a job for the given thread ID. 
 */
// 根据线程ID，找到相应作业
struct job *job_find(struct queue *qp,pthread_t id){
    struct job *jp;
    // 加读锁
    if(pthread_rwlock_rdlock(&qp->q_lock)!=0){
        return(NULL);
    }
    // 查找作业
    for(jp=qp->q_head;jp!=NULL;jp=jp->j_next){
        if(pthread_equal(jp->j_id,id)){
            break;
        }
    }
    // 解锁
    pthread_rwlock_unlock(&qp->q_lock);
    // 返回
    return(jp);
}