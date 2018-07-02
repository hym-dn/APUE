#include<limits.h>
#include<string.h>

static char envbuf[ARG_MAX];

extern char **environ;

char *getenv(const char *name){
    int i,len;
    // 名字长度
    len=strlen(name);
    // 遍历、获取环境变量
    for(i=0;environ[i]!=NULL;i++){
        // 比较环境变量名称
        if((strncmp(name,environ[i],len)==0)&&
            (environ[i][len]=='=')){
            // 拷贝环境变量内容
            strcpy(envbuf,&environ[i][len+1]); 
            // 返回环境变量内容
            return(envbuf);
        }
    }
    // 没有找到
    return(NULL);
}