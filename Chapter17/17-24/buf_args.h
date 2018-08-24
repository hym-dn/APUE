#include"../../apue.h"

// 定义最大参数个数
#define MAXARGC 50 /* max number of arguments in buf */
// 定义空白空格
#define WHITE " \t\n" /* white space for tokenizing arguments */

/**
 * buf[] contains white-space-separated arguments. We convert it to an
 * argv-style array of pointers, and call the user's function (optfunc)
 * to process the array. We return -1 if there's a problem parsing buf,
 * else we return whatever optfunc() returns. Note that user's buf[]
 * array is modified (nulls placed after each token).
 */
int buf_args(char *buf,int (*optfunc)(int,char**)){
    char *ptr,*argv[MAXARGC];
    int argc;
    // 分割字符串
    if(strtok(buf,WHITE)==NULL){ /* an argv[0] is required */
        return(-1);
    }
    // 保存第一子串
    argv[argc=0]=buf;
    // 获取分割的其它字串
    while((ptr=strtok(NULL,WHITE))!=NULL){
        // 溢出
        if(++argc>=MAXARGC-1){ /* -1 for room for NULL at end */
            return(-1);
        }
        // 保存
        argv[argc]=ptr;
    }
    // 设置结束
    argv[++argc]=NULL;
    // 调用回调分析参数
    /**
     * Since argv[] pointers point into the user's buf[],
     * user's function can just copy the pointers,even
     * though argv[] array will disappear on return.
     */
    return((*optfunc)(argc,argv));
}