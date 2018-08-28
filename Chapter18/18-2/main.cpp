#include"../../apue.h"
#include<termios.h>

// 使用屏蔽标志获取或设置一个值
int main(void){
    // 终端属性结构体
    struct termios term;
    // 获取标准输入的终端属性
    if(tcgetattr(STDIN_FILENO,&term)<0){ // 失败
        err_sys("tcgetattr error");
    }
    // 打印终端字符长度
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
    // 清除全部字符长度标志
    term.c_cflag&=~CSIZE; /* zero out the bits */
    // 设置8位字符长度
    term.c_cflag|=CS8; /* set 8 bits/byte */
    // 设置当前标准输入终端属性
    if(tcsetattr(STDIN_FILENO,TCSANOW,&term)<0){
        err_sys("tcsetattr error");
    }
    // 退出
    exit(0);
}