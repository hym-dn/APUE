#include"../../apue.h"

//定义最大参数个数
#define MAXARGC 50
//定义空白空格
#define WHITE " \t\n"

int buf_args(char *buf,int (*optfunc)(int,char**)){
    char *ptr,*argv[MAXARGC];
    int argc;
    //分割字符串
    if(strtok(buf,WHITE)==NULL){
        return(-1);
    }
    //保存第一子串
    argv[argc=0]=buf;
    //获取分割的其它字串
    while((ptr=strtok(NULL,WHITE))!=NULL){
        //溢出
        if(++argc>=MAXARGC-1){
            return(-1);
        }
        //保存
        argv[argc]=ptr;
    }
    //设置结束
    argv[++argc]=NULL;
    //调用回调分析参数
    return((*optfunc)(argc,argv));
}

