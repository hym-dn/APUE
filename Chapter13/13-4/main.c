#include"../../apue.h"
#include<syslog.h>
#include<errno.h>

// 锁文件
extern int lockfile(int);

// 单例运行
extern int already_running(void);

// 重读配置文件
void reread(void){
    /* ... */
}

// 信号中断
void sigterm(int signo){
    // 记录日志
    syslog(LOG_INFO,"got SIGTERM; exiting");
    // 退出
    exit(0);
}

// 信号挂起
void sighup(int signo){
    // 记录日志
    syslog(LOG_INFO,"Re-reading configuration file");
    // 重读
    reread();
}

// 主函数
int main(int argc,char *argv[]){
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
     * Handle signals of interest.
     */
    sa.sa_handler=sigterm; // 设置捕获函数
    sigemptyset(&sa.sa_mask); // 清空屏蔽字
    sigaddset(&sa.sa_mask,SIGHUP); // 增加SIGHUP信号
    sa.sa_flags=0;
    if(sigaction(SIGTERM,&sa,NULL)<0){
        // 输出日志
        syslog(LOG_ERR,"can't catch SIGTERM: %s",strerror(errno));
        // 退出
        exit(1);
    }
    sa.sa_handler=sighup;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask,SIGTERM);
    sa.sa_flags=0;
    if(sigaction(SIGHUP,&sa,NULL)<0){
        // 输出日志
        syslog(LOG_ERR,"can't catch SIGHUP: %s",strerror(errno));
        // 退出
        exit(1);
    }
    /**
     * Proceed with the rest of the daemon.
     */
    /* ... */
    exit(0);
}