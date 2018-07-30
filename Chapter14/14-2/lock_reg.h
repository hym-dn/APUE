#include"../../apue.h"
#include<fcntl.h>

int lock_reg(int fd,int cmd,int type,off_t offset,int whence,off_t len){
    // 声明文件锁
    struct flock lock;
    // 锁类型
    lock.l_type=type; /* F_RDLCK,F_WRLOCK,F_UNLCK */
    // 偏移起始地址
    lock.l_start=offset; /* byte offset,relative to l_whence */
    // 偏移起始位置
    lock.l_whence=whence; /* SEEK_SET,SEEK_CUR,SEEK_END */
    // 长度
    lock.l_len=len; /* #bytes (0 means to EOF) */
    // 上锁
    return(fcntl(fd,cmd,&lock));
}