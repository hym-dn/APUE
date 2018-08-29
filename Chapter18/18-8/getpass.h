#include<signal.h>
#include<stdio.h>
#include<termios.h>

// 最大口令长度
#define MAX_PASS_LEN 8 /* max #chars for user to enter */

char *getpass(const char *prompt){
    // 口令缓冲区
    static char buf[MAX_PASS_LEN+1]; /* null byte at end */
    char *ptr;
    // 信号集、原始信号集
    sigset_t sig,osig;
    // 终端属性、原始终端属性
    struct termios ts,ots;
    // 文件流
    FILE *fp;
    int c;
    // 以读写方式打开当前进程所使用的终端
    if((fp=fopen(ctermid(NULL),"r+"))==NULL){
        return(NULL);
    }
    // 设置终端缓冲区
    setbuf(fp,NULL);
    // 清空信号集
    sigemptyset(&sig);
    // 向信号集中添加信号
    sigaddset(&sig,SIGINT); /* block SIGINT */
    sigaddset(&sig,SIGTSTP); /* block SIGTSTP */
    // 设置信号屏蔽字
    sigprocmask(SIG_BLOCK,&sig,&osig); /* and save mask */
    // 获取终端属性
    tcgetattr(fileno(fp),&ts); /* save tty state */
    // 保存终端属性
    ots=ts; /* structure copy */
    // 不进行回显
    ts.c_lflag&=~(ECHO|ECHOE|ECHOK|ECHONL);
    // 设置终端属性
    tcsetattr(fileno(fp),TCSAFLUSH,&ts);
    // 向终端输入提示
    fputs(prompt,fp);
    // 循环读取输入
    ptr=buf;
    while((c=getc(fp))!=EOF&&c!='\n'){
        if(ptr<&buf[MAX_PASS_LEN]){
            *ptr++=c;
        }
    }
    *ptr=0; /* null terminate */
    // 输出换行
    putc('\n',fp); /* we echo a newline */
    // 恢复终端属性
    tcsetattr(fileno(fp),TCSAFLUSH,&ots); /* restore TTY state */
    // 恢复信号屏蔽
    sigprocmask(SIG_SETMASK,&osig,NULL); /* restore mask */
    // 关闭终端流
    fclose(fp); /* done with /dev/tty */
    // 返回
    return(buf);
}