#include"../../apue.h"
#include<termios.h>
#include<errno.h>

static struct termios save_termios;
static int ttysavefd=-1;
static enum{RESET,RAW,CBREAK} ttystate=RESET;

//将终端设置为cbreak模式
int tty_cbreak(int fd){
    int err;
    struct termios buf;
    //如果当前终端为处于重置状态
    if(ttystate!=RESET){
        errno=EINVAL;
        return(-1);
    }
    //获取当前终端信息
    if(tcgetattr(fd,&buf)<0){
        return(-1);
    }
    //存储终端信息
    save_termios=buf;
    //屏蔽回显、规范模式
    buf.c_lflag&=~(ECHO|ICANON);
    //设置接收限制
    buf.c_cc[VMIN]=1;
    buf.c_cc[VTIME]=0;
    //设置终端信息
    if(tcsetattr(fd,TCSAFLUSH,&buf)<0){
        return(-1);
    }
    //检验设置是否成功
    if(tcgetattr(fd,&buf)<0){
        err=errno;
        tcsetattr(fd,TCSAFLUSH,&save_termios);
        errno=err;
        return(-1);
    }
    if((buf.c_lflag&(ECHO|ICANON))||buf.c_cc[VMIN]!=1||
       buf.c_cc[VTIME]!=0){
        tcsetattr(fd,TCSAFLUSH,&save_termios);
        errno=EINVAL;
        return(-1);
    }
    //记录相关状态
    ttystate=CBREAK;
    ttysavefd=fd;
    //返回
    return(0);
}

//将终端设置为raw模式
int tty_raw(int fd){
    int err;
    struct termios buf;
     //如果当前终端为处于重置状态
    if(ttystate!=RESET){
        errno=EINVAL;
        return(-1);
    }
    //获取当前终端信息
    if(tcgetattr(fd,&buf)<0){
        return(-1);
    }
    //存储终端信息
    save_termios=buf;
    //关闭回显、规范模式、扩充输入字符以及终端产生信号
    buf.c_lflag&=~(ECHO|ICANON|IEXTEN|ISIG);
    //关闭接到BREAK时产生SIGINT、输入的CR转换为NL、打开
    //输入奇偶校验、剥除输入字符的第8位以及使启动/停止输出控制流起作用
    buf.c_iflag&=~(BRKINT|ICRNL|INPCK|ISTRIP|IXON);
    //关闭字符大小屏蔽、进行奇偶校验
    buf.c_cflag&=~(CSIZE|PARENB);
    //设置8位数据位
    buf.c_cflag|=CS8;
    //关闭输出处理
    buf.c_oflag&=~(OPOST);
    //设置接收限制
    buf.c_cc[VMIN]=1;
    buf.c_cc[VTIME]=0;
    //设置终端信息
    if(tcsetattr(fd,TCSAFLUSH,&buf)<0){
        return(-1);
    }
    //检验设置是否成功
    if(tcgetattr(fd,&buf)<0){
        err=errno;
        tcsetattr(fd,TCSAFLUSH,&save_termios);
        errno=err;
        return(-1);
    }
    if((buf.c_lflag&(ECHO|ICANON|IEXTEN|ISIG))||
       (buf.c_iflag&(BRKINT|ICRNL|INPCK|ISTRIP|IXON))||
       (buf.c_cflag&(CSIZE|PARENB|CS8))!=CS8||
       (buf.c_oflag&OPOST)||buf.c_cc[VMIN]!=1||
       buf.c_cc[VTIME]!=0){
        tcsetattr(fd,TCSAFLUSH,&save_termios);
        errno=EINVAL;
        return(-1);
    }
    //记录相关状态
    ttystate=RAW;
    ttysavefd=fd;
    //返回
    return(0);
}

//重置终端模式
int tty_reset(int fd){
    if(ttystate==RESET){
        return(0);
    }
    if(tcsetattr(fd,TCSAFLUSH,&save_termios)<0){
        return(-1);
    }
    ttystate=RESET;
    return(0);
}

void tty_atexit(void){
    if(ttysavefd>0){
        tty_reset(ttysavefd);
    }
}

struct termios *tty_termios(void){
    return(&save_termios);
}
