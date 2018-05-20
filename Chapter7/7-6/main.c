#include"../../apue.h"
#include<setjmp.h>

static void f1(int,int,int,int);
static void f2(void);
static jmp_buf jmpbuffer;

static int globval; // 全局变量

int main(void){
    int autoval; // 自动变量
    register int regival; // 寄存器变量
    volatile int volaval; // 易失变量
    static int statval; // 静态变量
    // 赋值
    globval=1;
    autoval=2;
    regival=3;
    volaval=4;
    statval=5;
    // 设置跳转标记
    if(setjmp(jmpbuffer)!=0){
        // 打印变量信息
        printf("after longjmp:\n");
        printf("globval=%d,autoval=%d,regival=%d,"
            "volaval=%d,statval=%d\n",globval,autoval,
            regival,volaval,statval);
        // 退出
        exit(0);
    }
    /**
     *  Change variables after setjmp, but before longjmp.
     */
    // 设置跳转标记后，更改变量值
    globval=95;
    autoval=96;
    regival=97;
    volaval=98;
    statval=99;
    // 调用函数
    f1(autoval,regival,volaval,statval); /* never returns */
    // 退出
    exit(0);
}

static void f1(int i,int j,int k,int l){
    printf("in f1():\n");
    printf("globval=%d,autoval=%d,regival=%d,"
        "volaval=%d,statval=%d\n",globval,i,
        j,k,l);
    f2();
}

static void f2(void){
    longjmp(jmpbuffer,1);
}