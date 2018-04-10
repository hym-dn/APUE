#include"../../apue.h"
#include<fcntl.h>

char buf1[]="abcdefghij";
char buf2[]="ABCDEFGHIJ";

int main(void){
    // 声明
    int fd;
    // 创建文件
    if((fd=creat("file.hole",FILE_MODE))<0){
        err_sys("creat error");
    }
    // 写入10个字节
    if(write(fd,buf1,10)!=10){
        err_sys("buf1 write error");
    }
    /* offset now=10 */
    // 设置偏移量为距文件头16384个字节
    if(lseek(fd,16384,SEEK_SET)==-1){
        err_sys("lseek error");
    }
    /* offset now=16384 */
    // 写入10个字节
    if(write(fd,buf2,10)!=10){
        err_sys("buf2 write error");
    }
    /* offset now=16394 */
    // 退出
    exit(0);
}