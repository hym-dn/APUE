#include<pwd.h>
#include<stddef.h>
#include<string.h>

struct passwd *getpwnam(const char *name){
    struct passwd *ptr;
    // 反绕口令文件
    setpwent();
    // 遍历口令文件，找到指定用户名称的口令信息
    while((ptr=getpwent())!=NULL){
        if(strcmp(name,ptr->pw_name)==0){
            break; /* found a match */
        }
    }
    // 关闭口令文件
    endpwent();
    // 返回口令信息
    return(ptr); /* ptr is NULL if no match found*/
}