#include"../../apue.h"

int main(void){
    char name[L_tmpnam],line[MAXLINE];
    FILE *fp;
    // 第一个临时名称
    printf("%s\n",tmpnam(NULL)); /* first temp name */
    // 第二个临时名称
    tmpnam(name); /* second temp name */
    printf("%s\n",name);
    // 创建临时文件
    if((fp=tmpfile())==NULL){ /* create temp file */
        err_sys("tmpfile error");
    }
    // 写临时文件
    fputs("one line of output\n",fp); /* write to temp file */
    // 回滚
    rewind(fp); /* then read it back */
    // 获取临时文件内容
    if(fgets(line,sizeof(line),fp)==NULL){
        err_sys("fgets error");
    }
    fputs(line,stdout); /* print the line we wrote */
    // 退出
    exit(0);
}