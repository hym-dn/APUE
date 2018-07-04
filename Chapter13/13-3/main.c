#include"../../apue.h"
#include<pthread.h>
#include<syslog.h>

// 信号屏蔽字
sigset_t mask;

// 单例运行
extern int already_running(void);

// 重读配置文件
void reread(void){
    /* ... */
}

// 线程函数
void *thr_fn(void *arg){
    int err,signo;
    // 无限循环，等待信号触发
    for(;;){
        // 等待信号发生
        err=sigwait(&mask,&signo); // sigwait等待期间会解除相应的屏蔽
        if(err!=0){
            // 日志
            syslog(LOG_ERR,"sigwait failed");
            // 退出
            exit(1);
        }
        // 信号分类
        switch(signo){
            // 挂起信号
            case SIGHUP:
                // 输出日志
                syslog(LOG_INFO,"Re-reading configuration file");
                // 重读配置
                reread();
                // 跳出
                break;
            // 中断信号
            case SIGTERM:
                // 输出日志
                syslog(LOG_INFO,"got SIGTERM; exiting");
                // 退出
                exit(0);
            // 默认
            default:
                syslog(LOG_INFO,"unexpected signal %d\n",signo);
        }
    }
    // 返回
    return(0);
}

int main(int argc,char *argv[]){
    int err;
    pthread_t tid;
    char *cmd;
    struct sigaction sa;
    // 如果'/'没有找到
    if((cmd=strrchr(argv[0],'/'))==NULL){
        cmd=argv[0];
    }
    // 如果找到了，截取命令
    else{
        cmd++;
    }
    /**
     * Become a daemon. 
     */
    // 成为守护进程
    daemonize(cmd);
    /**
     * Make sure only one copy of the daemon is running.
     */ 
    // 确认单例运行
    if(already_running()){
        // 日志
        syslog(LOG_ERR,"daemon already running");
        // 退出
        exit(1);
    }
    /**
     * Restore SIGHUP default and block all signals.
     */
    sa.sa_handler=SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=0;
}