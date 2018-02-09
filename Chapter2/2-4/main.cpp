#include"../../apue.h"
#include<errno.h>
#include<limits>

//打开限制
#ifdef OPEN_MAX
static long openmax=OPEN_MAX;
#else
static long openmax=0;
#endif //OPEN_MAX

//猜测限制
#define OPEN_MAX_GUESS 256

long open_max(){
    //尚未定义打开限制
    if(0==openmax){
        //清除错误
        errno=0;
        //获取限制
        if((openmax=sysconf(_SC_OPEN_MAX))<0){
            //未定义限制
            if(errno==0){
                openmax=OPEN_MAX_GUESS;
            }
            //出错
            else{
                err_sys("sysconf error for _SC_OPEN_MAX");
            }
        }
        //返回
        return(openmax);
    }
}