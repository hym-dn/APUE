#include"../17-21/opend.h"

// 定义错误消息
char errmsg[MAXLINE];
// 定义打开标志
int oflag;
// 定义路径名称
char *pathname;

int main(void){
    // 读取数量
    int nread;
    // 行缓冲区
    char buf[MAXLINE];
    // 循环读
    for(;;){ /* read arg buffer from client,process request */
        // 读
        if((nread=read(STDIN_FILENO,buf,MAXLINE))<0){ // 失败
            err_sys("read error on stream pipe");
        }
        // 文件尾(连接断开)
        else if(nread==0){
            break; /* client has close the stream pipe */
        }
        // 打开并且发送指定的文件描述符
        request(buf,nread,STDOUT_FILENO);
    }
    // 退出
    exit(0);
}