#include"../../apue.h"

int main(void){
    int int1,int2;
    char line[MAXLINE];
    // 循环从标准输入读
    while(fgets(line,MAXLINE,stdin)!=0){ // 读取成功
        // 读取数字成功
        if(sscanf(line,"%d%d",&int1,&int2)==2){
            // 标准输出
            if(printf("%d\n",int1+int2)==EOF){
                err_sys("printf error");
            }
        }
        // 读取数字失败
        else{
            if(printf("invalid args\n")==EOF){
                err_sys("printf error");
            }
        }
    }
    // 退出
    exit(0);
}
