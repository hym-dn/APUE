#include"../../apue.h"

int main(void){
    struct stat statbuf;
    /* turn on set-group-ID and turn off group-execute */
    // 获取foo文件的信息结构
    if(stat("foo",&statbuf)<0){
        err_sys("stat error for foo");
    }
    // 关闭foo的组执行，并且打开fool的设置组ID
    if(chmod("foo",(statbuf.st_mode&~S_IXGRP)|S_ISGID)<0){
        err_sys("chmod error for foo");
    }
    /* set absolute mode to "rw-r--r--" */
    // 设置绝对模式
    if(chmod("bar",S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)<0){
        err_sys("chmod error for bar");
    }
    // 退出
    exit(0);
}