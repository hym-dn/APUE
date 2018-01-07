#include<signal.h>
#include<stdio.h>
#include<termios.h>

//最大密码长度
#define MAX_PASS_LEN 8

char *getpass(const char *prompt){
    static char buf[MAX_PASS_LEN+1];
    char *ptr;
    sigset_t sig,osig;
    struct termios ts,ots;
    FILE *fp;
    int c;
    //打开当前进程使用的终端
    if((fp=fopen(ctermid(NULL),"r+"))==NULL){
        return(NULL);
    }
    //清空终端缓冲区
    setbuf(fp,NULL);
    //清空信号
    sigemptyset(&sig);
    //添加信号
    sigaddset(&sig,SIGINT);
    sigaddset(&sig,SIGTSTP);
    //阻塞信号
    sigprocmask(SIG_BLOCK,&sig,&osig);
    //获取终端信息
    tcgetattr(fileno(fp),&ts);
    //保存终端信息
    ots=ts;
    //
    ts.c_lflag&=~(ECHO|ECHOE|ECHOK|ECHONL);
    //设置终端信息
    tcsetattr(fileno(fp),TCSAFLUSH,&ts);
    //向终端输入提示
    fputs(prompt,fp);
    //循环从终端读取输入
    ptr=buf;
    while((c=getc(fp))!=EOF&&c!='\n'){
        if(ptr<&buf[MAX_PASS_LEN]){
            *ptr++=c;
        }
    }
    *ptr=0;
    //输出换行
    putc('\n',fp);
    //恢复终端属性
    tcsetattr(fileno(fp),TCSAFLUSH,&ots);
    //恢复信号屏蔽
    sigprocmask(SIG_SETMASK,&osig,NULL);
    //关闭终端流
    fclose(fp);
    //返回读取密码
    return(buf);
}
