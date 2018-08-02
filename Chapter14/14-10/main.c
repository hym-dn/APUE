#include"../../apue.h"
#include<stropts.h>

#define BUFFSIZE 4096

int main(void){
    int n,flag;
    char ctlbuf[BUFFSIZE],datbuf[BUFFSIZE];
    struct strbuf ctl,dat;
    // 设置流控制消息缓冲区
    ctl.buf=ctlbuf;
    ctl.maxlen=BUFFSIZE;
    // 这是数据消息缓冲区
    dat.buf=datbuf;
    dat.maxlen=BUFFSIZE;
    //
    for(;;){
        flag=0; /* return any message */
        // 获取标准输入流中消息
        if((n=getmsg(STDIN_FILENO,&ctl,&dat,&flag))<0){
            err_sys("getmsg error");
        }
        // 打印读取信息
        fprintf(stderr,"flag=%d,ctl.len=%d,dat.len=%d\n",
            flag,ctl.len,dat.len);
        // 如果数据位为0
        if(dat.len==0){
            exit(0);
        }
        // 如果数据位非0
        else if(dat.len>0){
            // 向标准输出写
            if(write(STDOUT_FILENO,dat.buf,dat.len)!=dat.len){
                err_sys("write error");
            }
        }
    }
}