#include"../../apue.h"
#include<fcntl.h>
#include<utime.h>

int main(int argc,char *argv[]){
    // 声明变量
    int i,fd;
    struct stat statbuf;
    struct utimbuf timebuf;
    // 遍历文件
    for(i=1;i<argc;i++){
        // 获取文件信息
        if(stat(argv[i],&statbuf)<0){ /* fetch current times*/
            // 输出错误
            err_ret("%s: stat error",argv[i]);
            // 继续
            continue;
        }
        // 以读写、清除方式打开指定文件
        if((fd=open(argv[i],O_RDWR|O_TRUNC))<0){ /* truncate */
            // 输出错误
            err_ret("%s: open error",argv[i]);
            // 继续
            continue;
        }
        // 关闭文件描述符
        close(fd);
        // 设置访问、修改时间
        timebuf.actime=statbuf.st_atime;
        timebuf.modtime=statbuf.st_mtime;
        // 设置时间
        if(utime(argv[i],&timebuf)<0){ /* reset times */
            // 输出出错
            err_ret("%s: utime error",argv[i]);
            // 继续
            continue;
        }
    }
    // 退出
    exit(0);
}