#include"opend.h"

char errmsg[MAXLINE];
int oflag;
char *pathname;

int main(void){
    int nread;
    char buf[MAXLINE];
    //
    for(;;){
        //标准输入读
        if((nread=read(STDIN_FILENO,buf,MAXLINE))<0){
            err_sys("read error on stream pipe");
        }
        //读取成功
        else if(nread==0){
            break;
        }
        //
        request(buf,nread,STDOUT_FILENO);
    }
    //退出
    exit(0);
}
