#include"../../apue.h"
#include<fcntl.h>

#define RWRWRW (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

int main(void){
    // 取消全部屏蔽字
    umask(0);
    // 创建文件
    if(creat("foo",RWRWRW)<0){
        err_sys("creat error for foo");
    }
    // 启用屏蔽字
    umask(S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    // 创建文件
    if(creat("bar",RWRWRW)<0){
        err_sys("creat error for bar");
    }
    // 退出
    exit(0);
}