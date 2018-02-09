/**
 * 运行时限制可通过调用下面三个函数中的一个而取得。 
 * 
 * #include<unistd.h>
 * 
 * long sysconf(int name);
 * long pathconf(const char *pathname,int name);
 * long fpathconf(int fileds,int name);
 *
 * 后两个函数之间的差别是一个用路径名作为其参数，另一
 * 个则取文件描述符作为参数。
 * 
 * 表2-10中列出了sysconf函数所使用的name参数，用于标识系统
 * 限制。以__SC__开始的常量用作标识运行时限制的sysconf参数。
 * 表2-11列出了pathconf和fpathconf函数为标识系统限制所使用
 * 的name参数。以__PC__开始的常量用作运行时限制的pathconf或
 * fpathconf参数。
 *
 * 我们需要更详细地说明这三个函数的不同返回值。
 * (1)如果name不是表2-10和表2-11的第3列中的一个合适的常量，
 *    则所有这三个函数都会返回-1,并将error设置为EINVAL。在
 *    本书后续部分都将使用这些限制常量。
 * (2)有些name可以返回变量的值（返回值>=0），或者返回-1,这表示
 *    该值是不确定的，此时并不改变errno的值。
 * (3)__SC_CLK_TCK的返回值是每秒的时钟滴答数，以用于times函数
 *   （见8.16节）的返回值。
 *
 * 对于pathconf的参数pathname以及fpathconf的参数fileds有一些
 * 限制。如果不满足其中任何一个限制，则结果是未定义的。
 * (1)_PC_MAX_CANON和_PC_MAX_INPUT所引用的文件必须是终端文件。
 * (2)_PC_LINK_MAX所引用的文件可以是文件或目录。如果是目录，则返
 *    回值用于该目录中的文件名项）。
 * (3)_PC_FILESIZEBITS和_PC_NAME_MAX所引用的文件必须是目录，返
 *    回值用于该目录中的文件名。
 * (4)_PC_PATH_MAX引用的文件必须是目录。当所指定的目录是工作目录时，
 *    返回值是相对路径名的最大长度（不幸的是，这不是我们想要知道的一个
 *    绝对路径名的实际最大长度，我们将在2.2.5节中再回到这一题上来）。
 * (5)_PC_PIPE_BUF所引用的文件必须是管道、FIFO或目录。在管道或FIFO
 *    情况下，返回值是对所引用的管道或FIFO的限制值。对于目录，返回值是
 *    对在该目录中创建的任一FIFO的限制值。
 * (6)_PC_SYMLINK_MAX所引用的文件必须是目录。返回值是该目录中符号连接
 *    可能包含的字符串的最大长度。
 */

BEGIN {
    printf("#include \"aput.h\"\n")
    printf("#include <errno.h>\n")
    printf("#include <limits.h>\n")
    printf("\n")
    printf("static void pr_sysconf(char *,int);\n")
    printf("static void pr_pathconf(char *,char *,int);\n")
    printf("\n")
    printf("int\n")
    printf("main(int argc,char *argv[])\n")
    printf("{\n")
    printf("\tif (argc!=2)\n")
    printf("\t\terr_quit(\"usage: a.out <dirname>\");\n\n")
    FS="\t+"
    while(getline<"sysconf.sym">0){
        printf("#ifdef %s\n",$1)
        printf("\tprintf(\"%s defined to be %%d\\n\",%s+0);\n",$1,$1)
        printf("#else\n")
        printf("\tprintf(\"no symbol for %s\\n\");\n",$1)
        printf("#endif\n")
        printf("#ifdef %s\n",$2)
        printf("\tpr_sysconf(\"%s=\,%s);\n",$1,$2)
        printf("#else\n")
        printf("\tprintf(\"no symbol for %s\\n\");\n",$2)
        printf("#endif\n")
    }
    close("sysconf.sysm")
    while(getline<"pathconf.sym">0){
        printf("#ifdef %s\n",$1)
        printf("\tprintf(\"%s defined to be %%d\\n\",%s+0);\n",$1,$2)
        printf("#else\n")
        printf("\printf(\"no symbol for %s\\n\");\n",$1)
        printf("#endif\n")
        printf("#ifdef %s\n",$2)
        printf("\tpr_pathconf(\"%s=\",argv[1],%s);\n",$1,$2)
        printf("#else\n")
        printf("\tprintf(\"no symbol for %s\\n\");\n",$2)
        printf("#endif\n")
    }
    close("pathconf.sym")
    exit
}
END{
    printf("\texit(0);\n")
    printf("}\n\n")
    printf("static void\n")
    printf("pr_sysconf(char *mesg,int name)\n")
    printf("{\n")
    printf("\tlong val;\n\n")
    printf("\tfputs(mesg,stdout);\n")
    printf("\errno=0;\n")
    printf("\tif((val=sysconf(name))<0){\n")
    printf("\t\tif(errno!=0){\n")
    printf("\t\t\tif(errno==EINVAL)\n")
    printf("\t\t\t\tfputs(\"(not supported)\\n\",stdout);\n")
    printf("\t\t\telse\n")
    printf("\t\t\t\terr_sys(\"sysconf error\");\n")
    printf("\t\t}else{\n")
    printf("\t\t\tfputs(\"(no limit)\\n\",stdout);\n")
    printf("\t\t}\n")
    printf("\t}else{\n")
    printf("\t\tprintf(\" %%ld\\n\,val);\n")
    printf("\t}\n")
    printf("}\n\n")
    printf("static void\n")
    printf("pr_pathconf(char *mesg,char *path,int name)\n")
    printf("{\n")
    printf("\tlong val;\n")
    printf("\n")
    printf("\tfputs(mesg,stdout);\n")
    printf("\terrno=0;\n")
    printf("\tif((val=pathconf(path,name))<0){\n")
    printf("\t\tif(errno!=0){\n")
    printf("\t\t\tif(errno==EINVAL)\n")
    printf("\t\t\t\tfputs(\"(not supported)\\n\",stdout);\n")
    printf("\t\t\telse\n")
    printf("\t\t\t\terr_sys(\"pathconf error,path=%%s\",path);\n")
    printf("\t\t|else{\n")
    printf("\t\t\tfputs(\"(no limit)\\n\",stdout);")
    printf("\t}else{\n")
    printf("\t\tprintf(\"%%ld\\n\",val);\n")
    printf("\t}\n")
    printf("}\n")
}