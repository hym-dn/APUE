#include"../../apue.h"
#include<termios.h>

int main(void){
    //定义终端信息结构体
    struct termios term;
    //获取标准输入的终端信息
    if(tcgetattr(STDIN_FILENO,&term)<0){
        err_sys("tcgetattr error");
    }
    //打印当前终端数据位数
    switch(term.c_cflag&CSIZE){
    case CS5:
        printf("5 bits/byte\n");
        break;
    case CS6:
        printf("6 bits/byte\n");
        break;
    case CS7:
        printf("7 bits/byte\n");
        break;
    case CS8:
        printf("8 bits/byte\n");
        break;
    default:
        printf("unknown bits/byte\n");
    }
    //清除数据位长度
    term.c_cflag&=~CSIZE;
    //设置8位数据位
    term.c_cflag|=CS8;
    //设置当前标准输入终端信息
    if(tcsetattr(STDIN_FILENO,TCSANOW,&term)<0){
        err_sys("tcsetattr error");
    }
    //退出
    exit(0);
}

