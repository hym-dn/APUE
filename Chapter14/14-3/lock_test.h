#include"../../apue.h"
#include<fcntl.h>

pid_t lock_test(int fd,int type,off_t offset,int whence,off_t len){
    // 定义锁
    struct flock lock;
    // 设置锁类型
    lock.l_type=type; /* F_RDLCK or F_WRLCK */
    // 设置偏移字节
    lock.l_start=offset; /* byte offset,relative to l_whence */
    // 设置起始位置
    lock.l_whence=whence; /* SEEK_SET,SEEK_CUR,SEEK_END */
    // 锁长度
    lock.l_len=len; /* #bytes (0 means to EOF)*/
    // 获取记录锁
    if(fcntl(fd,F_GETLK,&lock)<0){
        err_sys("fcntl error");
    }
    // 如果记录锁不存在
    if(lock.l_type==F_UNLCK){
        return(0); /* false,region isn't locked by another proc */
    }
    // 返回加锁进程
    return(lock.l_pid); /* true,return pid of lock owner */
}

// 功能宏
#define is_read_lockable(fd,offset,whence,len) \
    (lock_test((fd),F_RDLCK,(offset),(whence),(len))==0)
#define is_write_lockable(fd,offset,whence,len) \
    (lock_test((fd),F_WRLCK,(offset),(whence),(len))==0)