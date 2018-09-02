#include"../../apue.h"
#include<termios.h>
#ifndef TIOCGWINSZ
#include<sys/ioctl.h> /* for struct winsize*/
#endif

#ifdef LINUX
#define OPTSTR "+d:einv"
#else
#define OPTSTR "d:einv"
#endif

static void set_noecho(int); /* at the end of this file*/
void do_driver(char *); /* in the file driver.c */
void loop(int,int); /* in the file loop.c */

int main(int argc,char *argv[]){
    int fdm,c,ignoreeof,interactive,noecho,verbose;
    pid_t pid;
    char *driver;
    char slave_name[20];
    struct termios orig_termios;
    struct winsize size;
    // 初始化相关标记变量
    interactive=isatty(STDIN_FILENO); // 标准输入是否为终端
    ignoreeof=0;
    noecho=0;
    verbose=0;
    driver=NULL;
    /**
     *  getopt()用来分析命令行参数。参数argc和argv是由main()传递的参数个数
     *  和内容。参数optstring则代表欲处理的选项字符串。此函数会返回在argv中下
     *  一个的选项字母，此字母会对应参数optstring中的字母。如果选项字符串里的
     *  字母后接着冒号“:”，则表示还有相关的参数，全域变量optarg即会指向此额外
     *  参数。如果getopt()找不到符合的参数则会印出错信息，并将全域变量optopt
     *  设为“?”字符，如果不希望getopt()印出错信息，则只要将全域变量opterr设
     *  为0即可。
     */
    // 解析所有的命令字符,设置相应标志
    opterr=0; /* don't want getopt() writing to stderr */
    while((c=getopt(argc,argv,OPTSTR))!=EOF){
        switch(c){
        case 'd': /* driver for stdin/stdout */
            driver=optarg; //记录而外参数
            break;
        case 'e': /* noecho for slave pty's line discipline */
            noecho=1; // 并不回显伪终端行规范
            break;
        case 'i': /* ignore EOF on standard input */
            ignoreeof=1; // 忽略标准输入的EOF
            break;
        case 'n': /* not interactive */
            interactive=0; // 取消交互
            break;
        case 'v': /* verbose */
            verbose=1; // 显示虚拟机的运行信息
            break;
        case '?':
            err_quit("unrecongized option: -%c",optopt);
        }
    }
    // 如果参数下一个检索位置超出范围
    if(optind>=argc){
        err_quit("usage: pty [ -d driver -einv ] program [ arg ...]");
    }
    // 不与终端进行交互
    if(interactive){ /* fetch current termios and window size */
        // 获取标准输入属性
        if(tcgetattr(STDIN_FILENO,&orig_termios)<0){
            err_sys("tcgetattr error on stdin");
        }
        // 获取标准输入窗口尺寸
        if(ioctl(STDIN_FILENO,TIOCGWINSZ,(char*)&size)<0){
            err_sys("TIOCGWINSZ error");
        }
        // 开启伪终端主、从设备
        pid=pty_fork(&fdm,slave_name,sizeof(slave_name),
            &orig_termios,&size);
    }
    // 非交互
    else{
        // 开启伪终端主、从设备
        pid=pty_fork(&fdm,slave_name,sizeof(slave_name),
            NULL,NULL);
    }
    // 进程创建失败
    if(pid<0){
        err_sys("fork error");
    }
    // 子进程
    else if(0==pid){ /* child */
        // 不回显
        if(noecho){
            // 设置不回显
            set_noecho(STDIN_FILENO); /* stdin is slave pty */
            // 执行用户输入的命令
            if(execvp(argv[optind],&argv[optind])<0){
                err_sys("can't execute: %s",argv[optind]);
            }
        }
    }
    // 父进程
    // 显示虚拟机的运行信息
    if(verbose){
        // 打印从设备名称
        fprintf(stderr,"slave name = %s\n",slave_name);
        // 驱动非空
        if(driver!=NULL){
            fprintf(stderr,"driver = %s\n",driver);
        }
    }
    // 可交互，但驱动为空
    if(interactive&&driver==NULL){
        // 设置标准输入为规范终端
        if(tty_raw(STDIN_FILENO)<0){ /* user's tty to raw mode */
            err_sys("tty_raw error");
        }
        // 注册退出函数
        if(atexit(tty_atexit)<0){ /* reset user's tty on exit */
            err_sys("atexit error");
        }
    }
    // 驱动存在
    if(driver){
        do_driver(driver); /* changes our stdin/stdout */
    }
    // 拷贝伪终端到标准输入、标准输出
    loop(fdm,ignoreeof); /* copies stdin -> ptym,ptym -> stdout */
    // 退出
    exit(0);
}

// 关闭从设备回显
static void set_noecho(int fd){
    struct termios stermios;
    // 获取指定终端信息
    if(tcgetattr(fd,&stermios)<0){
        err_sys("tcgetattr error");
    }
    // 关闭回显
    stermios.c_lfalg&=~(ECHO|ECHOE|ECHOK|ECHONL);
    /**
     * Also turn off NL to CR/NL mapping on output.
     */
    // 关闭CR/NL映射
    stermios.c_oflag&=~(ONLCR);
    // 设置终端属性
    if(tcsetattr(fd,TCSANOW,&stermios)<0){
        err_sys("tcsetattr error");
    }
}