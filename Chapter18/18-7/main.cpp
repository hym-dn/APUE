#include"../../apue.h"
#include"../18-6/ttyname.h"

int main(void){
    char *name;
    // 标准输入
    // 终端设备
    if(isatty(0)){
        // 获取标准输入名称
        name=ttyname(0);
        // 获取失败
        if(name==NULL){
            name="undefined";
        }
    }
    // 非终端设备
    else{
        name="not a tty";
    }
    // 打印标准输入名称
    printf("fd 0: %s\n",name);
    // 标准输出
    // 终端设备
    if(isatty(1)){
        // 获取标准输出名称
        name=ttyname(1);
        // 获取失败
        if(name==NULL){
            name="undefined";
        }
    }
    // 非终端设备
    else{
        name="not a tty";
    }
    // 打印标准输出名称
    printf("fd 1: %s\n",name);
    // 标准出错
    // 终端设备
    if(isatty(2)){
        // 获取标准出错名称
        name=ttyname(2);
        // 获取失败
        if(name==NULL){
            name="undefined";
        }
    }
    // 非终端设备
    else{
        name="not a tty";
    }
    // 打印标准出错名称
    printf("fd 2: %s\n",name);
    // 退出
    exit(0);
}
