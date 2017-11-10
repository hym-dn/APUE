#include"../../apue.h"

int main(void){
   //变量
   int c;
   //循环读
   while((c=getc(stdin))!=EOF){
       //循环写
       if(putc(c,stdout)==EOF){
           err_sys("output error");
       }
   }
   //读失败
   if(ferror(stdin)){
       err_sys("input error");
   }
   //退出
   exit(0);
}

