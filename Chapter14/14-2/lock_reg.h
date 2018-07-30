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

// 加读锁
#define read_lock(fd,offset,whence,len) \
    lock_reg((fd),F_SETLK,F_RDLCK,(offset),(whence),(len))
#define readw_lock(fd,offset,whence,len) \
    lock_reg((fd),F_SETLKW,F_RDLCK,(offset),(whence),(len))
// 加写锁
#define write_lock(fd,offset,whence,len) \
    lock_reg((fd),F_SETLK,F_WRLCK,(offset),(whence),(len))
#define writew_lock(fd,offset,whence,len) \
    lock_reg((fd),F_SETLKW,F_WRLCK,(offset),(whence),(len))
// 解锁
#define un_lock(fd,offset,whence,len) \
    lock_reg((fd),F_SETLK,F_UNLCK,(offset),(whence),(len))