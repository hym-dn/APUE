#include"../../apue.h"
#include<fcntl.h>
#include<stropts.h>

/**
 * 创建一个流管道连接到服务器。
 * name - 管道到名称。
 * 如果函数执行成功返回管道文件描述符，
 * 否则返回值小于0。
 */
int cli_conn(const char *name){
    //文件描述符
    int fd;
    //打开指定的管道
    if((fd=open(name,O_RDWR))<0){
        return(-1);
    }
    //检测是否为流设备
    if(isastream(fd)==0){
        //关闭文件描述符
        close(fd);
        //返回错误
        return(-2);
    }
    //返回文件描述符
    return(fd);
}
