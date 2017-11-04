#include"../../apue.h"
#include<ctype.h>

int main(void){
    //字符
    int c;
    //如果输入未结束
    while((c==getchar())!=EOF){
        //输入大写
        if(isupper(c)){
            //转小写
            c=tolower(c);
        }
        //输出字符
        if(putchar(c)==EOF){
            err_sys("output error");
        }
        //遇到回车冲洗
        if(c=='\n'){
            fflush(stdout);
        }
    }
    //退出
    exit(0);
}
