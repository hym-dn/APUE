#include"../../apue.h"
#include<termios.h>
#ifdef TIOCGWINSZ
#include<sys/ioctl.h>
#endif

//用fork调用打开主设备和从设备
pid_t pty_fork(int *ptrfdm,
    char *slave_name,int slave_namesz,
    const struct termios *slave_termios,
    const struct winsize *slave_winsize){
    int fdm,fds;
    pid_t pid;
    char pts_name[20];
    //打开伪终端主设备
    if((fdm=ptym_open(pts_name,sizeof(pts_name)))<0){
        err_sys("can't open master pty: %s,error %d",pts_name,fdm);
    }
    //如果期望返回伪终端从设备名称
    if(slave_name!=NULL){
        /**
         * Return name of slave. Null terminate to handle case
         * where strlen(pts_name) > slave_namesz.
         */
        //保存伪终端从设备名称
        strncpy(slave_name,pts_name,slave_namesz);
        //防止字符串溢出
        slave_name[slave_namesz-1]='\0';
    }
    //创建子进程
    if((pid=fork())<0){
        return(-1);
    }
    //子进程
    else if(0==pid){
        //创建新的会话，并将当前进程设置为会话的组长
        if(setsid()<0){
            err_sys("setsid error");
        }
        /**
         * System V acquires controlling terminal on open().
         */
        //打开伪终端从设备
        if((fds=ptys_open(pts_name))<0){
            err_sys("can't open slave pty");
        }
        //关闭伪终端主设备
        close(fdm); /* all done with master in child */
#if defined(TIOCSCTTY)
        /**
         * TIOCSCTTY is the BSD way to acquire a controlling terminal.
         */
        //分配一个控制终端
        if(ioctl(fds,TIOCSCTTY,(char*)0)<0){
            err_sys("TIOCSCTTY error");
        }
#endif
        /**
         * Set slave's termios and window size. 
         */
        //设置终端属性、窗口尺寸
        if(slave_termios!=NULL){
            if(tcsetattr(fds,TCSANOW,slave_termios)<0){
                err_sys("tcsetattr error on slave pty");
            }
        }
        if(slave_winsize!=NULL){
            if(ioctl(fds,TIOCSWINSZ,slave_winsize)<0){
                err_sys("TIOCSWINSZ error on slave pty");
            }
        }
        /**
         * Slave becomes stdin/stdout/stderr of child.
         */
        //将从设备设置为当前进程的标准输入、标准输出已经标准出错.
        if(dup2(fds,STDIN_FILENO)!=STDIN_FILENO){
            err_sys("dup2 error to stdin");
        }
        if(dup2(fds,STDOUT_FILENO)!=STDOUT_FILENO){
            err_sys("dup2 error to stdout");
        }
        if(dup2(fds,STDERR_FILENO)!=STDERR_FILENO){
            err_sys("dup2 error to stderr");
        }
        //如果设置出错
        if((fds!=STDIN_FILENO)&&(fds!=STDOUT_FILENO)&&(
            fds!=STDERR_FILENO)){
            close(fds);
        }
        //返回
        return(0); /* child returns 0 just like fork() */
    }
    //父进程
    else{
        *ptrfdm=fdm; /* return fd of master */
        return(pid); /* parent returns pid of child */        
    }
}