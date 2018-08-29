#include"../../apue.h"
#include<termios.h>
#include<errno.h>

static struct termios save_termios; // 存储的终端属性信息
static int ttysavefd=-1; // 存储的终端描述符
static enum{RESET,RAW,CBREAK} ttystate=RESET; // 终端状态

// 将终端设置为cbreak模式
int tty_cbreak(int fd){ /* put terminal into a cbreak mode */
    int err;
    // 终端属性结构体
    struct termios buf;
    // 如果当前状态并未处于初始状态
    if(ttystate!=RESET){
        errno=EINVAL;
        return(-1);
    }
    // 获取当前终端信息
    if(tcgetattr(fd,&buf)<0){
        return(-1);
    }
    // 存储终端信息
    save_termios=buf; /* structure copy */
    // 屏蔽回显、规范模式
    /**
     * Echo off,canonical mode off.
     */
    buf.c_lflag&=~(ECHO|ICANON);
    // 设置接收限制
    /**
     * Case B: 1 byte at a time,no timer.
     */
    buf.c_cc[VMIN]=1;
    buf.c_cc[VTIME]=0;
    // 设置终端信息
    if(tcsetattr(fd,TCSAFLUSH,&buf)<0){
        return(-1);
    }
    // 检验设置是否成功
    /**
     * Verify that the changes stuck. tcsetattr can return 0 on
     * partial success.
     */
    if(tcgetattr(fd,&buf)<0){
        err=errno;
        tcsetattr(fd,TCSAFLUSH,&save_termios);
        errno=err;
        return(-1);
    }
    if((buf.c_lflag&(ECHO|ICANON))||buf.c_cc[VMIN]!=1||
       buf.c_cc[VTIME]!=0){
        /**
         * Only some of the changes were made. Restore the 
         * original settings.
         */
        tcsetattr(fd,TCSAFLUSH,&save_termios);
        errno=EINVAL;
        return(-1);
    }
    // 记录相关状态
    ttystate=CBREAK;
    ttysavefd=fd;
    // 返回
    return(0);
}

// 将终端设置为raw模式
int tty_raw(int fd){ /* put terminal into a raw mode */
    int err;
    // 终端属性结构体
    struct termios buf;
    // 如果终端处于非初始状态
    if(ttystate!=RESET){
        errno=EINVAL;
        return(-1);
    }
    // 获取终端属性
    if(tcgetattr(fd,&buf)<0){
        return(-1);
    }
    // 存储终端属性
    save_termios=buf; /* structure copy */
    // 关闭回显、规范模式、扩充输入字符以及终端产生信号
    /**
     * Echo off,canonical mode off,extended input
     * processing off,signal chars off.
     */
    buf.c_lflag&=~(ECHO|ICANON|IEXTEN|ISIG);
    // 接到BREAK时不产生SIGINT、关闭输入的CR转换为NL、关闭输入奇偶校验、
    // 剥除输入字符的第8位、关闭输出流控
    /**
     * No SIGINT on BREAK,CR-to-NL off,input parity check off,
     * don't strip 8th bit on input,output flow control off.
     */
    buf.c_iflag&=~(BRKINT|ICRNL|INPCK|ISTRIP|IXON);
    // 清楚数据位尺寸、关闭奇偶校验
    buf.c_cflag&=~(CSIZE|PARENB);
    // 设置8位数据位
    /**
     * Set 8 bits/char.
     */
    buf.c_cflag|=CS8;
    // 关闭输出处理
    /**
     * Output processing off.
     */
    buf.c_oflag&=~(OPOST);
    // 设置接收限制
    /**
     * Case B: 1 byte at a time, no timer.
     */
    buf.c_cc[VMIN]=1;
    buf.c_cc[VTIME]=0;
    // 设置终端信息
    if(tcsetattr(fd,TCSAFLUSH,&buf)<0){
        return(-1);
    }
    // 检验设置是否成功
    /**
     * Verify that the changes stuck. tcsetattr can return 0 on
     * paritial success.
     */
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
    // 记录相关状态
    ttystate=RAW;
    ttysavefd=fd;
    // 返回
    return(0);
}

// 重置终端模式
int tty_reset(int fd){ /* restore terminal's mode */
    if(ttystate==RESET){
        return(0);
    }
    if(tcsetattr(fd,TCSAFLUSH,&save_termios)<0){
        return(-1);
    }
    ttystate=RESET;
    return(0);
}

// 终端退出清理函数
void tty_atexit(void){ /* can be set up by atexit(tty_atexit) */
    if(ttysavefd>0){
        tty_reset(ttysavefd);
    }
}

// 返回原始终端状态
struct termios *tty_termios(void){ /* let caller see original tty state */
    return(&save_termios);
}