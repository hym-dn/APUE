#include"../../apue.h"
#include<sys/acct.h>

#ifdef HAS_SA_STAT
#define FMT "%-*.*s e = %6ld, chars = %7ld, stat = %3u: %c %c %c %c\n" 
#else
#define FMT "%-*.*s e = %6ld, chars = %7ld, %c %c %c %c\n"
#endif

#ifndef HAS_ACORE
#define ACORE 0
#endif

#ifndef HAS_AXSIG
#define AXSIG 0
#endif

// 将comp_t转换为unsigned long
static unsigned long compt2ulong(comp_t comptime){ /* convert comp_t to unsigned long */
    unsigned long val;
    int exp;
    // 13位的小数
    val=comptime&0x1fff; /* 13-bit fraction */
    // 3位指数
    exp=(comptime>>13)&7; /* 3-bit exponent (0-7) */
    // 计算值
    while(exp-->0){
        val*=8;
    }
    // 返回值
    return(val);
}