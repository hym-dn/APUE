#include"../../apue.h"

int main(){
    int n,int1,int2;
    // 行缓冲区
    char line[MAXLINE];
    // 标准输入读取一行
    while((n=read(STDIN_FILENO,line,MAXLINE))>0){ // 读取成功
        // 追加字符串结束标识
        line[n]=0;
        // 格式化读取字符串中的数据
        if(sscanf(line,"%d%d",&int1,&int2)==2){
            // 回写行缓冲区中
            sprintf(line,"%d\n",int1+int2);
            // 计算长度
            n=strlen(line);
            // 写标准输出
            if(write(STDOUT_FILENO,line,n)!=n){
                err_sys("write error");
            }
        }
        // 提取数据失败
        else{
            // 输出错误
            if(write(STDOUT_FILENO,"invalid args\n",13)!=13){
                err_sys("write error");
            }
        }
    }
    //退出
    exit(0);
}
