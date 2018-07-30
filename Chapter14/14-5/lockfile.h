#include<unistd.h>
#include<fcntl.h>

int lockfile(int fd){
    // 锁结构
    struct flock fl;
    // 设置锁类型
    fl.l_type=F_WRLCK;
    // 设置偏移
    fl.l_start=0;
    // 设置起始位置
    fl.l_whence=SEEK_SET;
    // 设置长度
    fl.l_len=0; // 指明锁住整个文件
    // 加锁
    return(fcntl(fd,F_SETLK,&fl));
}