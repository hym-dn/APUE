#include"../../apue.h"
#include<errno.h>
#include<limits.h>

#ifdef PATH_MAX
static int pathmax=PATH_MAX;
#else
static int pathmax=0;
#endif

#define SUSV3 200112L

static long posix_version=0;

/* If PATH_MAX is indeterminate,no guarantee this is adequate */
#define PATH_MAX_GUESS 1024

char *path_alloc(int *sizep){ /* also return allocated size,if nonnull */
    char *ptr;
    int size;
    //取得当前系统版本
    if(posix_version==0)
        posix_version=sysconf(_SC_VERSION);
    //最大路径尚未获取
    if(pathmax==0){ /* first time through*/
        //清除错误
        errno=0;
        //获取根目录相对路径名的最大字节数，包括终止字符
        if((pathmax=pathconf("/",_PC_PATH_MAX))<0){
            //无错误
            if(errno==0)
                pathmax=PATH_MAX_GUESS; /* it's ideterminate */
            else
                err_sys("pathconf error for _PC_PATH_MAX");
        }
        //成功获取根目录相对路径的最大字节数，包括终止字符
        else{
            pathmax++; /* add one since it's relative to root*/
        }
    }
    if(posix_version<SUSV3)
        size=pathmax+1;
    else
        size=pathmax;
    //分配内存
    if((ptr=(char*)malloc(size))==NULL)
        err_sys("malloc error for pathname");
    //返回分配尺寸
    if(sizep!=NULL)
        *sizep=size;
    //返回
    return(ptr);
}

/*
//for debug
int main(void){
    int sizep=0;
    char *memory=path_alloc(&sizep);
    free(memory);
    printf("%d\n",sizep);
    return(0);
}
*/