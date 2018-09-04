#include"../../apue.h"
#include"apue_db.h"
#include<fcntl.h> /* open & db_open flags */ // 文件锁
#include<stdarg.h> // 可变参数函数
#include<errno.h> // 错误编号
#include<sys/uio.h> /* struct iovec */ // 批量IO

/**
 * Internal index file constants.
 * These are used to construct records in the
 * index file and data file.
 */
#define IDXLEN_SZ 4 /* index record length (ASSCII chars) */ // 索引长度
#define SEP ':' /* separator char in index record */ // 分隔符
#define SPACE ' ' /* space character */ // 空格符
#define NEWLINE '\n' /* newline character */ // 换行符

/**
 * The following definitions are hash chains and free
 * list chain in the index file.
 */
#define PTR_SZ 6 /* size of ptr field in hash chain */ // 散列表中指针的长度
#define PTR_MAX 999999 /* max file offset = 10**PTR_SZ-1 */ // 最大文件偏移
#define NHASH_DEF 137 /* default hash table size */ // 默认的哈希表大小
#define FREE_OFF 0 /* free list offfset in index file */ // 索引文件中空闲链表偏移量
#define HASH_OFF PTR_SZ /* hash table offset in index file */ // 索引文件中哈希表偏移量

typedef unsigned long DBHASH; /* hash values */ // 哈希值
typedef unsigned long COUNT; /* unsigned counter */ // 计数

/**
 * Library's private representation of the database.
 */
typedef struct{
    int idxfd; /* fd for index file */ // 索引文件描述符
    int datfd; /* fd for data file */ // 数据文件描述符
    char *idxbuf; /* malloc'ed buffer for index record */ // 索引缓冲区
    char *datbuf; /* malloc'ed buffer for data record */ // 数据缓冲区
    char *name; /* name db was opened under */ // 数据名称
    off_t idxoff; /* offset in index file of index record */ // 索引处于索引文件中的偏移量
                  /* key is at (idxoff + PTR_SZ + IDXLEN_SZ) */
    size_t idxlen; /* length of index record */ // 一条索引长度
                   /* excludes IDXLEN_SZ bytes at front of record */
                   /* includes newline at end of index record */
    off_t datoff; /* offset in data file of data record */ // 数据记录处于数据文件中的偏移量
    size_t datlen; /* length of data record */ // 数据记录的长度
                   /* includes newline at end */
    off_t ptrval; /* contents of chain ptr in index record */ // 索引文件中链表指针内容 ??
    off_t ptroff; /* chain ptr offset pointing to this idx record */ // 当前索引偏移
    off_t chainoff; /* offset of hash chain for this index record */ // 哈希链表偏移量
    off_t hashoff; /* offset in index file of hash table */ // 哈希表偏移量
    DBHASH nhash; /* current hash table size */ // 哈希表尺寸 
    COUNT cnt_delok; /* delete OK */ // 删除计数
    COUNT cnt_delerr; /* delete error */ // 删除错误??
    COUNT cnt_fetchok; /* fetch OK */ // 获取成功计数
    COUNT cnt_fetcherr; /* fetch error */ // 获取错误计数
    COUNT cnt_nextrec; /* nextrec */ // ??
    COUNT cnt_stor1; /* store: DB_INSERT,no empty,appended */ // ??
    COUNT cnt_stor2; /* store: DB_INSERT,found empty,reused */ // ??
    COUNT cnt_stor3; /* store: DB_REPLACE,diff len,appended */ // ??
    COUNT cnt_stor4; /* store: DB_REPLACE,same len,overwrote */ // ??
    COUNT cnt_storerr; /* store error */
}DB;

/**
 * Internal functions 
 * 内部函数声明
 */
static DB *_db_alloc(int);
static void _db_dodelete(DB*);
static int _db_find_and_lock(DB*,const char*,int);
static int _db_findfree(DB*,int,int);
static void _db_free(DB*);
static DBHASH _db_hash(DB*,const char*);
static char *_db_readdat(DB*);
static off_t _db_readidx(DB*,off_t);
static off_t _db_readptr(DB*,off_t);
static void _db_writedat(DB*,const char*,off_t,int);
static void _db_writeidx(DB*,const char*,off_t,int,off_t);
static void _db_writeptr(DB*,off_t,off_t);

/**
 * Open or create a database. Same arguments as open(2).
 * 打开创建一个数据库
 * pathname - 数据库名称
 * oflag - 打开标志
 * ... - 文件权限
 */
DBHANDLE db_open(const char *pathname,int oflag,...){
    DB *db;
    int len,mode;
    size_t i;
    char asciiptr[PTR_SZ+1];
    char hash[(NHASH_DEF+1)*PTR_SZ+2]; /* +2 for newline and null */
    struct stat statbuff;
    /**
     * Allocate a DB structure,and the buffers it needs.
     */
    len=strlen(pathname); // 获取数据库名称长度
    // 分配并初始化DB结构
    if((db=_db_alloc(len))==NULL){
        err_dump("db_open: _db_alloc error for DB");
    }
    // 设置哈希表尺寸
    db->nhash=NHASH_DEF; /* hash table size */
    // 索引文件中哈希表的偏移
    db->hashoff=HASH_OFF; /* offset in index file of hash table */
    // 存储数据库名称
    strcpy(db->name,pathname);
    strcat(db->name,".idx");
    // 创建数据库
    if(oflag&O_CREAT){
        // 获取可变参数
        va_list ap;
        va_start(ap,oflag);
        mode=va_arg(ap,int);
        va_end(ap);
        /**
         * Open index file and data file.
         */
        // 打开索引文件、数据文件
        db->idxfd=open(db->name,oflag,mode);
        strcpy(db->name+len,".dat"); // 修改后缀名称
        db->datfd=open(db->name,oflag,mode);
    }
    // 并非创建数据库
    else{
        /**
         * Open index file and data file.
         */
        // 打开索引文件、数据文件
        db->idxfd=open(db->name,oflag);
        strcpy(db->name+len,".dat"); // 修改后缀名称
        db->datfd=open(db->name,oflag);
    }
    // 如果索引、数据文件创建失败
    if(db->idxfd<0||db->datfd<0){
        _db_free(db);
        return(NULL);
    }
    // 如果创建并且截断数据库
    if((oflag&(O_CREAT|O_TRUNC))==(O_CREAT|O_TRUNC)){
        /**
         * If the database was created,we have to initialize
         * it. Write lock the entire file so that we can stat
         * it,check its size,and initialize it,atomically.
         */
        if(writew_lock(db->idxfd,0,SEEK_SET,0)<0){ //　非阻塞写锁
            err_dump("db_open: writew_lock error");
        }
        if(fstat(db->idxfd,&statbuff)<0){ // 获取索引文件属性
            err_sys("db_open: fstat error");
        }
        //　如果文件为空
        if(statbuff.st_size==0){
            /**
             * We have to build a list of (NHASH_DEF+1) chain
             * ptrs with a value of 0. The +1 is for the free
             * list pointer that precedes the hash table.
             */
            /**
             * 向文件中写【空闲链表指针】【链表指针】【链表指针】...
             */
            sprintf(asciiptr,"%*d",PTR_SZ,0); // %*d 表示人为指定长度，PTR_SZ就是长度
            // 初始化哈希表
            hash[0]=0;
            for(i=0;i<NHASH_DEF+1;i++){
                strcat(hash,asciiptr); // 拼接链表指针
            }
            strcat(hash,"\n"); // 拼接换行符
            i=strlen(hash);
            // 向索引文件写入hash表
            if(write(db->idxfd,hash,i)!=i){
                err_dump("db_open: index file init write error");
            }
        }
        //　解文件锁
        if(un_lock(db->idxfd,0,SEEK_SET,0)<0){
            err_dump("db_open: un_lock error");
        }
    }
    // 回滚文件
    db_rewind(db);
    // 返回
    return(db);
}

/**
 * Allocate & initialize a DB structure and its bufffers.
 * 分配初始化DB结构体及其缓冲区
 * namelen - 数据库名字长度
 */
static DB *_db_alloc(int namelen){
    DB *db;
    /**
     * Use calloc, to initialize the structure to zero.
     * 利用calloc分配一个ＤＢ结构，并且自动将内存初始化为０.
     */
    if((db=calloc(1,sizeof(DB)))==NULL){
        err_dump("_db_alloc: calloc error for DB");
    }
    // 初始化描述符为-1
    db->idxfd=db->datfd=-1; /* descriptors */
    /**
     * Allocate room for the name.
     * +5 for ".idx" or ".dat" plus null at end.
     * 为数据库名称分配空间
     */
    if((db->name=malloc(namelen+5))==NULL){
        err_dump("_db_alloc: malloc error for name.");
    }
    /**
     * Allocate an index buffer and a data buffer.
     * +2 for newline and null at end.
     * 分配索引、数据缓冲
     */
    if((db->idxbuf=malloc(IDXLEN_MAX+2))==NULL){
        err_dump("_db_alloc: malloc error for index buffer.");
    }
    if((db->datbuf=malloc(DATLEN_MAX+2))==NULL){
        err_dump("_db_alloc: malloc error for data buffer.");
    }
    // 返回数据库
    return(db);
}

/**
 * Relinquish access to the database.
 * 关闭指定的数据库
 */
void db_close(DBHANDLE h){
    _db_free((DB*)h); /* close fds,free buffers & struct */
}

/**
 * Free up a DB structure, and all the malloc'ed buffers it 
 * may point to. Also close the file descriptors if still open.
 * 释放DB数据库结构，包括结构中的缓冲区以及全部打开的文件描述符
 */
static void _db_free(DB *db){
    // 关闭打开的文件描述符
    if(db->idxfd>=0){
        close(db->idxfd);
    }
    if(db->datfd>=0){
        close(db->datfd);
    }
    // 释放缓冲区
    if(db->idxbuf!=NULL){
        free(db->idxbuf);
    }
    if(db->datbuf!=NULL){
        free(db->datbuf);
    }
    // 释放数据库名字
    if(db->name!=NULL){
        free(db->name);
    }
    // 释放DB
    free(db);
}

/**
 * Fetch a record. Return a pointer to the null-terminated data.
 * 获取一条记录
 * h - 数据库
 * key - 记录主键
 */
char *db_fecth(DBHANDLE h,const char *key){
    DB *db=h;
    char *ptr;
    // 查找数据库中制定记录
    if(_db_find_and_lock(db,key,0)<0){ // 查找失败
        ptr=NULL; /* error,record not found */
        db->cnt_fetcherr++; // 统计查找失败次数
    }
    // 查找到制定记录
    else{ // 查找成功
        ptr=_db_readdat(db); /* rturn pointer to data */
        db->cnt_fetchok++; // 统计查找成功次数
    }
    /**
     * Unlock the hash chain that _db_find_and_lock locked.
     * 解锁被_db_find_and_lock锁住的哈希链
     */
    if(un_lock(db->idxfd,db->chainoff,SEEK_SET,1)<0){
        err_dump("db_fetch: un_lock error");
    }
    // 返回数据
    return(ptr);
}

/**
 * Find the specified record. Called by db_delete,db_fetch,
 * and db_store. Returns with the hash chain locked. 
 * 查找指定的记录，并且对其加锁
 */
static int _db_find_and_lock(DB *db,const char *key,int writelock){
    off_t offset,nextoffset;
    /**
     * Calculate the hash value for this key, then calculate the 
     * byte offset of corresponding chain ptr in hash table.
     * This is where our search starts. First we calculate the 
     * offset in the hash table for this key.
     */
    db->chainoff=(_db_hash(db,key)*PTR_SZ)+db->hashoff; // 计算哈希链表偏移量
    db->ptroff=db->chainoff;
    /**
     * We lock the hash chain here. The caller must unlock it
     * when done. Note we lock and unlock only the first byte.
     * 对哈希链表加锁。注意，我们加、解锁只锁第一个字节
     */
    if(writelock){ // 写锁
        if(writew_lock(db->idxfd,db->chainoff,SEEK_SET,1)<0){
            err_dump("_db_find_and_lock: writew_lock error");
        }
    }else{ // 读锁
        if(readw_lock(db->idxfd,db->chainoff,SEEK_SET,1)<0){
            err_dump("_db_find_and_lock: readw_lock error");
        }
    }
    /**
     * Get the offset in the index file of first record
     * on the hash chain (can be 0).
     * 获取指定哈希链表中的第一个指针
     */
    offset=_db_readptr(db,db->ptroff);
    while(offset!=0){
        // 读取索引信息，并且返回下一个哈希链表指针
        nextoffset=_db_readidx(db,offset);
        // 如果找到指定的主键
        if(strcmp(db->idxbuf,key)==0){
            break; /* found a match */
        }
        // 记录当前索引偏移量
        db->ptroff=offset; /* offset of this (unequal) record */
        // 记录下一个索引偏移量
        offset=nextoffset; /* next one to compare */
    }
    /**
     * offset==0 on error (record not found);
     */
    return(offset==0?-1:0);
}

/**
 * Calculate the hash value for a key.
 * 计算哈希值
 * db - 数据库
 * key - 主键
 */
static DBHASH _db_hash(DB *db,const char *key){
    DBHASH hval=0;
    char c;
    int i;
    /**
     * 它将键中每一个ASCII字符乘以这个字符在字符串中
     * 以１开始的索引号，将这些结果加起来，除以散列表
     * 记录项数，将余数作为散列值
     */
    for(i=1;(c=*key++)!=0;i++){
        hval+=c*i; /* ascii char times its 1-based index */
    }
    return(hval%db->nhash);
}

/**
 * Read a chain ptr field from anywhere in the index file:
 * the free list pointer, a hash table chain ptr, or an 
 * index record chain ptr.
 * 读取一下三种不同链表指针中的任意一种：
 * 索引文件最开始处指向空闲链表中的第一条索引记录的指针;
 * 散列表中指向散列链的第一条索引记录的指针;
 * 存放在每条索引记录开始处、指向下一条记录的指针
 */
static off_t _db_readptr(DB *db,off_t offset){
    char asciiptr[PTR_SZ+1];
    // 定位文件
    if(lseek(db->idxfd,offset,SEEK_SET)==-1){
        err_dump("_db_readptr: lseek error to ptr field");
    }
    // 读文件
    if(read(db->idxfd,asciiptr,PTR_SZ)!=PTR_SZ){
        err_dump("_db_readptr: read error of ptr field");
    }
    // 增加结束符
    asciiptr[PTR_SZ]=0; /* null terminate */
    // 返回整型地址
    return(atol(asciiptr));
}

/**
 * Read the next index record. We start at the specified offset
 * in the index file. We read the index record into db->idxbuf
 * and replace the separators with null typets. If all is OK we
 * set db->datoff and db->datlen to the offset and length of the
 * corresponding data record in the data file.
 */
static off_t _db_readidx(DB *db,off_t offset){
    ssize_t i;
    char *ptr1,*ptr2;
    char assciiptr[PTR_SZ+1],asciilen[IDXLEN_SZ+1];
    struct iovec iov[2];

}