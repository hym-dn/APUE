#include"../../apue.h"
#include<errno.h>
#include<fcntl.h>

char buf[500000];

int main(void){
    int ntowrite,nwrite;
    char *ptr;
    // 读取标准输入
    ntowrite=read(STDIN_FILENO,buf,sizeof(buf));
    // 向标准出错写入读取字节数
    fprintf(stderr,"read %d bytes\n",ntowrite);
    // 设置标准输出为非阻塞
    set_fl(STDOUT_FILENO,O_NONBLOCK); /* set nonblocking */
    ptr=buf;
    // 如果读取的字节数量大于0
    while(ntowrite>0){
        // 清空错误
        errno=0;
        // 向标准输出写
        nwrite=write(STDOUT_FILENO,ptr,ntowrite);
        // 向标准出错写
        fprintf(stderr,"nwrite=%d,errno=%d\n",nwrite,errno);
        // 如果写的字节大于0
        if(nwrite>0){
            ptr+=nwrite;
            ntowrite-=nwrite;
        }
    }
    // 清除非阻塞标志
    clr_fl(STDOUT_FILENO,O_NONBLOCK); /* clear nonblocking */
    // 退出
    exit(0);
}