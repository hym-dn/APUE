#include<pthread.h>

// 消息结构体
struct msg{
    struct msg *m_next;
    /* ... more stuff here ...*/
    // 更多的内容
};

// 消息队列
struct msg *workq;
// 条件变量
pthread_cond_t qready=PTHREAD_COND_INITIALIZER;
// 互斥变量
pthread_mutex_t qlock=PTHREAD_MUTEX_INITIALIZER;

// 处理消息
void process_msg(void){
    // 消息结构体
    struct msg *mp;
    //
    for(;;){
        // 加锁
        pthread_mutex_lock(&qlock);
        // 队列为空
        while(workq==NULL){
            // 等待条件触发
            pthread_cond_wait(&qready,&qlock); // 解锁 等待 加锁
        }
        // 消息出队
        mp=workq;
        workq=mp->m_next;
        // 解锁
        pthread_mutex_unlock(&qlock);
        /* now process the message mp */
    }
}

// 消息入队
void enqueue_msg(struct msg *mp){
    // 上锁
    pthread_mutex_lock(&qlock);
    // 消息入队
    mp->m_next=workq;
    workq=mp;
    // 解锁
    pthread_mutex_unlock(&qlock);
    // 通知条件改变
    pthread_cond_signal(&qready);
}