#include<string.h>
#include<errno.h>
#include<pthread.h>
#include<stdlib.h>

extern char **environ;

pthread_mutex_t env_mutex;
static pthread_conce_t init_done=PTHREAD_ONCE_INIT;

// 线程初始化
static void thread_init(void){
    pthread_mutexattr_t attr;
    // 初始化互斥属性
    pthread_mutexattr_init(&attr);
    // 设置递归互斥属性
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
    // 初始化互斥量
    pthread_mutex_init(&env_mutex,&attr);
    // 销毁互斥属相
    pthread_mutexattr_destroy(&attr);
}

// 线程安全获取环境变量版本
int getenv_r(const char *name,char *buf,int buflen){
    int i,len,olen;
    // 确保多线程值调用一次thread_init
    pthread_once(&init_done,thread_init);
    // 获取名字长度
    len=strlen(name);
    // 上锁
    pthread_mutex_lock(&env_mutex);
    // 查找环境变量
    for(i=0;environ[i]!=NULL;i++){
        // 如果成功找到环境变量
        if((strncmp(name,environ[i],len)==0)&&
            (environ[i][len]=='=')){
            // 计算环境变量长度
            olen=strlen(&environ[i][len+1]);
            // 超出缓冲区长度
            if(olen>=buflen){
                // 解锁
                pthread_mutex_unlock(&env_mutex);
                // 返回
                return(ENOSPC);
            }
            // 拷贝环境变量
            strcpy(buf,&environ[i][len+1]);
            // 解锁
            pthread_mutex_unlock(&env_mutex);
            // 返回
            return(0);
        }
    }
    // 解锁
    pthread_mutex_unlock(&env_mutex);
    // 返回
    return(ENOENT);
}