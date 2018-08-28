#include<stdio.h>
#include<string.h>

// 终端名称缓冲区
static char ctermid_name[L_ctermid];

// 获取进程终端名称
char *ctermid(char *str){
    if(str==NULL){
        str=ctermid_name;
    }
    return(strcpy(str,"/dev/tty")); /* strcpy() return str */
}