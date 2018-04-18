#include"../../apue.h"
#include<fcntl.h>

int main(void){
    // 读写打开临时文件
    if(open("tempfile",O_RDWR|O_CREAT)<0){
        err_sys("open error"); // 失败
    }
    // 减少临时文件连接次数
    if(unlink("tempfile")<0){
        err_sys("unlink error"); // 失败
    }
    // 打印信息
    printf("file unlinked\n");
    // 休眠
    sleep(15);
    // 打印信息
    printf("done\n");
    // 退出
    exit(0);
}