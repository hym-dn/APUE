#include<termios.h>

// 检测输入文件描述符是否为终端
int isatty(int fd){
    struct termios ts;
    return(tcgetattr(fd,&ts)!=-1); /* true if no error (is a tty) */
}