#include"../../apue.h"

int main(){
    int n,int1,int2;
    //输入内容
    char line[MAXLINE];
    //标准输入读
    while((n==read(STDIN_FILENO,line,MAXLINE))>0){
        //追加字符串结束标识
        line[n]=0;
        //读取第一个、第二个输入
        if(sscanf(line,"%d%d",&int1,&int2)==2){
            //将和写回line中
            sprintf(line,"%d\n",int1+int2);
            //计算字符串长度
            n=strlen(line);
            //向标准输出输出字符串
            if(write(STDOUT_FILENO,line,n)!=n){
                err_sys("write error");
            }
        }
        //读取失败
        else{
            //输出错误
            if(write(STDOUT_FILENO,"invalid args\n",13)!=13){
                err_sys("write error");
            }
        }
    }
    //退出
    exit(0);
}
