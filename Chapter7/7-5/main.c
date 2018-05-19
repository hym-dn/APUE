#include"../../apue.h"
#include<setjmp.h>

#define TOK_ADD 5

// 跳转标记
jmp_buf jmpbuffer;

void do_line(char*);
void cmd_add(void);
int get_token(void);

// 入口程序
int main(void){
    // 存储一行输入内容
    char line[MAXLINE];
    // 设置跳转返回标记
    if(setjmp(jmpbuffer)!=0){
        printf("error");
    }
    // 从标准输入中读取一行内容
    while(fgets(line,MAXLINE,stdin)!=NULL){
        do_line(line); // 处理一行内容
    }
    // 退出
    exit(0);
}

// 存储输入内容指针
char *tok_ptr; /* global pointer for get_token() */

// 处理一行内容
void do_line(char *ptr){
    // 存储命令
    int cmd;
    // 记录输入内容指针
    tok_ptr=ptr;
    // 分析、获取命令
    while((cmd=get_token())>0){
        // 分析命令
        switch(cmd){ /* one case for each command. */
        case TOK_ADD:
            cmd_add(); // 处理命令
            break; // 跳出
        }
    }
}

// 添加指令
void cmd_add(void){
    int token;
    token=get_token();
    if(token<0){ /* an error has occurred */
        longjmp(jmpbuffer,1);
    }
    /* rest of processing for this command */
}

// 分析、返回指令
int get_token(){
    /* fetch next token from line pointed to by tok_ptr */
}