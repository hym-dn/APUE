#include"../../apue.h"

void pr_stdio(const char *,FILE *);

int main(void){
    FILE *fp;
    // 向标准输出输出信息
    fputs("enter any character\n",stdout);
    // 从标准输入读取一个字符
    if(getchar()==EOF){ // 失败
        err_sys("getchar error");
    }
    // 向标准出错输出信息
    fputs("one line to standard error\n",stderr);
    // 打印标准输入、标准输出以及标准出错的缓冲状态信息
    pr_stdio("stdin",stdin);
    pr_stdio("stdout",stdout);
    pr_stdio("stderr",stderr);
    // 打开指定文件
    if((fp=fopen("/etc/motd","r"))==NULL){ // 打开失败
        err_sys("fopen error");
    }
    // 读取一个字符
    if(getc(fp)==EOF){ // 读取失败
        err_sys("getc error");
    }
    // 打印缓冲状态信息
    pr_stdio("/etc/motd",fp);
    // 退出
    exit(0);
}

void pr_stdio(const char *name,FILE *fp){
    // 打印流的名字
    printf("stream=%s ",name);
    // 打印缓冲状态信息
    /**
     * The following is nonportable. 
     */
    // 非缓冲
    if(fp->_IO_file_flags&_IO_UNBUFFERED){
        printf("unbuffered"); 
    }
    // 行缓冲
    else if(fp->_IO_file_flags&_IO_LINE_BUF){
        printf("line buffered");
    }
    // 全缓冲
    else{
        printf("fully buffered");
    }
    // 打印缓冲缓冲区尺寸
    printf(",buffer size = %d\n",fp->_IO_buf_end-fp->_IO_buf_base);
}