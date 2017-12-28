#include"../../apue.h"
#include<termios.h>

int main(void){
    //定义终端
    struct termios term;
    //_PC_VDISABLE值
    long vdisable;
    //检测标准输入是否为终端设备
    if(isatty(STDIN_FILENO)==0){
        err_quit("standard input is not a terminal device");
    }
    //获取标准输入的_PC_VDISABLE值
    if((vdisable=fpathconf(STDIN_FILENO,_PC_VDISABLE))<0){
        err_quit("pfathconf error or _POSIX_VDISABLE not in effect");
    }
    //获取标准输入的终端信息结构
    if(tcgetattr(STDIN_FILENO,&term)<0){
        err_sys("tcgetattr error");
    }
    //修改终端结构
    term.c_cc[VINTR]=vdisable;//禁用中断键
    term.c_cc[VEOF]=2;//将文件结束符设置为Control-B
    //重新设置终端信息结构
    //TCSANOW：不等数据传输完毕就立即改变属性。
    //TCSADRAIN：等待所有数据传输结束才改变属性。
    //TCSAFLUSH：等待所有数据传输结束,清空输入输出缓冲区才改变属性。
    if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&term)<0){
        err_sys("tcsetattr error");
    }
    //退出
    exit(0);
}
