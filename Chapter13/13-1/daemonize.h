#include"../../apue.h"
#include<syslog.h>
#include<fcntl.h>
#include<sys/resource.h>

void daemonize(const char *cmd){
    int i,fd0,fd1,fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;
    /**
     * Clear file creation mask. 
     */
    // 1.首先要做的是调用umask将文件模式创建屏蔽字设置为0
    umask(0);
    /**
     * Get Maximum number of file descriptors. 
     */
    // 获取系统中文件描述符最大限制
    if(getrlimit(RLIMIT_NOFILE,&rl)<0){
        err_quit("%s: can't get file limit",cmd);
    }
    /**
     * Become a session leader to lose controlling TTY.
     */
    // 2.调用fork，然后使父进程退出(exit)
    if((pid=fork())<0){
        err_quit("%s: can't fork",cmd);
    }else if(pid!=0){ /* parent */ // 父进程
        exit(0); // 退出
    }
    // 3.调用setsig以创建一个新会话
    setsid();
    /**
     * Ensure future opens won't allocate controlling TTYs. 
     */
    // 防止守护进程取得控制终端
    sa.sa_handler=SIG_IGN; // 忽略
    sigemptyset(&sa.sa_mask);  // 清空信号屏蔽字
    sa.sa_flags=0; // 重置标志位
    if(sigaction(SIGHUP,&sa,NULL)<0){ // 忽略SIGHUP信号
        err_quit("%s: can't ignore SIGHUP");
    }
    if((pid=fork())<0){ // 创建子进程
        err_quit("%s: can't fork",cmd);
    }else if(pid!=0){ /* parent */ // 父进程
        exit(0);
    }
    /**
     * Change the current working directory to the root so
     * we won't prevent file systems from being unmounted. 
     */
    // 4.将当前工作目录更改为根目录
    if(chdir("/")<0){
        err_quit("%s: can't change directory to /");
    }
    /**
     * Close all open file descriptors.
     */
    // 5.关闭不再需要的文件描述符
    if(rl.rlim_max==RLIM_INFINITY){
        rl.rlim_max=1024;
    }
    for(i=0;i<rl.rlim_max;i++){
        close(i);
    }
    /**
     * Attach file descriptors 0,1 and 2 to /dev/null.
     */
    // 6.打开/dev/null使其具文件描述符0、1和2
    fd0=open("/dev/null",O_RDWR);
    fd1=dup(0);
    fd2=dup(0);
    /**
     * Initialize the log file
     */
    // 初始化日志系统
    openlog(cmd,LOG_CONS,LOG_DAEMON);
    if(fd0!=0||fd1!=1||fd2!=2){
        syslog(LOG_ERR,"unexpected file descriptors %d %d %d",
            fd0,fd1,fd2);
        exit(1);
    }
}

void main(void){
    daemonize("hello");
    pause();
}