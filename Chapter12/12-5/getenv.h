#include<limits.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>

static pthread_key_t key;
static pthread_once_t init_done=PTHREAD_ONCE_INIT;
pthread_mutex_t env_mutex=PTHREAD_MUTEX_INITIALIZER; // 初始化静态互斥量

extern char **environ;

// 线程初始化
static void thread_init(void){
    // 创建主键
    pthread_key_create(&key,free);
}

// 获取环境变量
char *getenv(const char *name){
    int i,len;
    char *envbuf;
    // 线程只初始化一次
    pthread_once(&init_done,thread_init);
    // 加锁
    pthread_mutex_lock(&env_mutex);
    // 获取主键绑定的数据
    envbuf=(char *)pthread_getspecific(key);
    // 如果数据尚未绑定，则创建
    if(envbuf==NULL){
        // 创建数据
        envbuf=malloc(ARG_MAX);
        // 如果创建失败
        if(envbuf==NULL){
            // 解锁
            pthread_mutex_unlock(&env_mutex);
            // 返回
            return(NULL);
        }
        // 绑定主键
        pthread_setspecific(key,envbuf);
    }
    // 名字长度
    len=strlen(name);
    // 遍历、获取环境变量
    for(i=0;environ[i]!=NULL;i++){
        // 比较环境变量名称
        if((strncmp(name,environ[i],len)==0)&&
            (environ[i][len]=='=')){
            // 拷贝环境变量内容
            strcpy(envbuf,&environ[i][len+1]); 
            // 返回环境变量内容
            return(envbuf);
        }
    }
    // 解锁
    pthread_mutex_unlock(&env_mutex);
    // 返回
    return(NULL);
}