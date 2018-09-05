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
    size_t idxlen; /* length of index record */ // 索引长度
                   /* excludes IDXLEN_SZ bytes at front of record */
                   /* includes newline at end of index record */
    off_t datoff; /* offset in data file of data record */ // 数据记录处于数据文件中的偏移量
    size_t datlen; /* length of data record */ // 数据记录的长度
                   /* includes newline at end */
    off_t ptrval; /* contents of chain ptr in index record */ // 当前记录中链表指针内容
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
    char hash[(NHASH_DEF+1)*PTR_SZ+2]; /* +2_db_writeptr for newline and null */
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
 * 从索引文件中的指定偏移量处读取索引记录。
 */
static off_t _db_readidx(DB *db,off_t offset){
    ssize_t i;
    char *ptr1,*ptr2;
    char assciiptr[PTR_SZ+1],asciilen[IDXLEN_SZ+1];
    struct iovec iov[2];
    /**
     * Position index file and record the offset. db_nextrec calls 
     * us with==0,meaning read from current offset.We still need to 
     * call lseek to record the current offset.
     * 使索引文件指针移动到指定偏移位置处
     */
    if((db->idxoff=lseek(db->idxfd,offset,offset==0?SEEK_CUR:SEEK_SET))==-1){
        err_dump("_db_readidx: lseek_error");
    }
    /**
     * Read the ascii chain ptr and the ascii length at the front of 
     * the index record. This tells us the remaining size of the index 
     * record.
     * 读取链表指针，以及索引长度
     */
    iov[0].iov_base=assciiptr;
    iov[0].iov_len=PTR_SZ;
    iov[1].iov_base=asciilen;
    iov[1].iov_len=IDXLEN_SZ;
    if((i=readv(db->idxfd,&iov[0],2))!=PTR_SZ+IDXLEN_SZ){ // 读取失败
        // 读到结束符
        if(i==0&&offset==0){
            return(-1); /* EOF for db_nextrec */
        }
        // 输出错信息
        err_dump("_db_readidx: readv error of index record");
    }
    /**
     * This is our return value; always >= 0
     */
    // 追加结束符
    assciiptr[PTR_SZ]=0; /* null terminate */
    // 记录中索引指针内容
    db->ptrval=atol(assciiptr); /* offset of next key in chain */
    // 追加结束符
    asciilen[IDXLEN_SZ]=0; /* null terminate */
    // 记录索引长度，并且检测合法性
    if((db->idxlen=atoi(asciilen))<IDXLEN_MIN||db->idxlen>IDXLEN_MAX){
        err_dump("_db_readidx: invalid length");
    }
    /**
     * Now read the actual index record. We read it into the key
     * buffer that we malloced when we opened the database.
     * 读取索引不定长部分
     */
    // 读取索引不定长部分
    if((i=read(db->idxfd,db->idxbuf,db->idxlen))!=db->idxlen){
        err_dump("_db_readidx: read error of index record");
    }
    // 检验是否以换行符结尾
    if(db->idxbuf[db->idxlen-1]!=NEWLINE){ /* sanity check */
        err_dump("_db_readidx: missing newline");
    }
    // 替换换行符
    db->idxbuf[db->idxlen-1]=0; /* replace newline with null */
    /**
     * Find the separators in the index record.
     * 根据分隔符，划分索引数据，键：数据偏移：数据长度
     */
    if((ptr1=strchr(db->idxbuf,SEP))==NULL){
        err_dump("_db_readidx: missing first separator");
    }
    *ptr1++=0; /* replace SEP with null */
    if((ptr2=strchr(ptr1,SEP))==NULL){
        err_dump("_db_readidx: missing second separator");
    }
    *ptr2++=0; /* replace SEP with null */
    if(strchr(ptr2,SEP)!=NULL){
        err_dump("_db_readidx: too many separators");
    }
    /**
     * Get the starting offset and length of the data record.
     * 保存数据记录的偏移量以及长度
     */
    if((db->datoff=atol(ptr1))<0){
        err_dump("_db_readidx: starting offset < 0");
    }
    if((db->datlen=atol(ptr2))<=0||db->datlen>DATLEN_MAX){
        err_dump("_db_readidx: invalid length");
    }
    // 返回下一条索引记录偏移量
    return(db->ptrval); /* return offset of next key in chain */
}

/**
 * Read the current data record into the data buffer.
 * Return a pointer to the null-terminated data buffer.
 * 读取数据文件中的数据
 */
static char *_db_readdat(DB *db){
    // 定位数据_db_writeptr
    if(lseek(db->datfd,db->datoff,SEEK_SET)==-1){
        err_dump("_db_readdat: lseek error");
    }
    // 读取数据
    if(read(db->datfd,db->datbuf,db->datlen)!=db->datlen){
        err_dump("_db_readdat: read error");
    }
    // 检验合法性
    if(db->datbuf[db->datlen-1]!=NEWLINE){ /* sanity check */
        err_dump("_db_readdat: missing newline");
    }
    // 替换换行符
    db->datbuf[db->datlen-1]=0; /* replace newline with null */
    // 返回数据记录指针
    return(db->datbuf);
}

/**
 * Delete the specified record.
 * 删除指定的记录
 */
int db_delete(DBHANDLE h,const char *key){
    // 声明当前操作所需变量
    DB *db=h;
    int rc=0; /* assume record will be found */
    // 查找指定主键的记录
    if(_db_find_and_lock(db,key,1)==0){ // 查找成功
        // 删除
        _db_dodelete(db);
        // 删除计数
        db->cnt_delok++;
    }else{ // 查找失败
        rc=-1; /* not found */
        // 删除失败计数
        db->cnt_delerr++; 
    }
    // 解锁
    if(un_lock(db->idxfd,db->chainoff,SEEK_SET,1)<0){
        err_dump("db_delete: un_lock error");
    }
    // 返回
    return(rc);
}

/**
 * Delete the current record specified by the DB structure.
 * This function is called by db_delete and db_store,after
 * the record has been located by _db_find_and_lock.
 * 从数据中删除当前记录
 */
static void _db_dodelete(DB *db){
    int i;
    char *ptr;
    off_t freeptr,saveptr;
    /**
     * Set data buffer and key to all blanks.
     * DB结构中的数据缓冲区用SPACE字符清空
     */
    for(ptr=db->datbuf,i=0;i<db->datlen-1;i++){
        *ptr++=SPACE;
    }
    *ptr=0; /*null terminate for _db_writedat */
    // DB结构中的索引缓冲区用SPACE字符清空
    ptr=db->idxbuf;
    while(*ptr){
        *ptr++=SPACE;
    }
    /**
     * We have to lock the free list.
     * 对空闲链表加锁
     */
    if(writew_lock(db->idxfd,FREE_OFF,SEEK_SET,1)<0){
        err_dump("_db_dodelete: wrietw_lock error");
    }
    /**
     * Write the data record with all blanks.
     * 擦除数据文件
     */
    _db_writedat(db,db->datbuf,db->datoff,SEEK_SET);
    /**
     * Read the free list pointer. Its value becomes the 
     * chain ptr field of the deleted index record. This
     * means the deleted record becomes the head of the 
     * free list.
     */
    // 读取空闲链表
    freeptr=_db_readptr(db,FREE_OFF);
    /**
     * Save the contents of index record chain ptr,
     * before it's rewritten by _db_writeidx.
     */
    // 存储当前记录的一下条链表指针内容
    saveptr=db->ptrval;
    /**
     * Rewrite the index record. This also rewrites the length
     * of the index record,the data offset, and the data length,
     * none of which has changed, but that's OK.
     * 重写索引文件中当前记录
     */
    _db_writeidx(db,db->idxbuf,db->idxoff,SEEK_SET,freeptr);
    /**
     * Write the new free list pointer
     * 写一个新的空闲链表
     */
    _db_writeptr(db,FREE_OFF,db->idxoff);
    /**
     * Rewrite the chain ptr that pointed to this record being
     * deleted. Recall that _db_find_and_lock sets db->ptroff 
     * to point to this chain ptr. We set this chain ptr to the 
     * contents of the deleted record's chain ptr, saveptr.
     * 重写被删除的链表
     */
    _db_writeptr(db,db->ptroff,saveptr);
    // 解锁
    if(un_lock(db->idxbuf,FREE_OFF,SEEK_SET,1)<0){
        err_dump("_db_dodelete: unlock error");
    }
}

/**
 * Write a data record. Called by _db_dodelete (to write
 * the record with blanks ) and db_store.
 * 写指定的数据到数据文件中
 */
static void _db_writedat(DB *db,const char *data,off_t offset,int whence){
    struct iovec iov[2];
    static char newline=NEWLINE;
    /**
     * If we're appending, we have to lock before doing the lseek
     * and write to make the two an atomic operation. If we're overwriting
     * an existing record, we don't have to lock.
     */
    // 如果当前是追加写
    if(whence==SEEK_END){
        // 上锁
        if(writew_lock(db->datfd,0,SEEK_SET,0)<0){ /* we're appending,lock entire file */
            err_dump("_db_writedat: writew_lock error");
        }
    }
    // 定位数据
    if((db->datoff=lseek(db->datfd,offset,whence))==-1){
        err_dump("_db_writedat: lseek error");
    }
    db->datlen=strlen(data)+1; /* datlen includes newline */
    // 写文件
    iov[0].iov_base=(char*)data;
    iov[0].iov_len=db->datlen-1;
    iov[1].iov_base=&newline;
    iov[1].iov_len=1;
    if(writev(db->datfd,&iov[0],2)!=db->datlen){
        err_dump("_db_writedat: writev error of data record");
    }
    // 如果当前是追加
    if(whence==SEEK_END){
        // 解锁
        if(un_lock(db->datfd,0,SEEK_SET,0)<0){
            err_dump("_db_writedat: unlock error");
        }
    }
}

/**
 * Write an index record. _db_writedat is called before
 * this function to set datoff and datlen fields in the
 * DB structure, which we need to write the index record.
 * 写索引记录到索引文件中
 */
static void _db_writeidx(DB *db,const char *key,off_t offset,
    int whence,off_t ptrval){
    struct iovec iov[2];
    char assiiptrlen[PTR_SZ+IDXLEN_SZ+1];
    int len;
    char *fmt;
    // 设置当前记录中下一个链表指针
    if((db->ptrval=ptrval)<0||ptrval>PTR_MAX){ // 非法
        err_quit("_db_writeidx: invalid ptr: %d",ptrval);
    }
    // 形成数据索引字符串
    if(sizeof(off_t)==sizeof(long long)){
        fmt="%s%c%lld%c%d\n";
    }else{
        fmt="%s%c%ld%c%d\n";
    }
    sprintf(db->idxbuf,fmt,key,SEP,db->datoff,SEP,db->datlen);
    if((len==strlen(db->idxbuf))<IDXLEN_MIN||len>IDXLEN_MAX){ // 数据索引字符串非法
        err_dump("_db_writeidx: invalid length");
    }
    sprintf(assiiptrlen,"%*ld%*d",PTR_SZ,ptrval,IDXLEN_SZ,len);
    /**
     * If we're appending, we have to lock before doing the lseek
     * and write to make the two an atomic operation. If we're overwriting
     * an existing record, we don't have to lock.
     * 追加上锁
     */
    if(whence==SEEK_END){ /* we're appending */
        if(writew_lock(db->idxfd,((db->nhash+1)*PTR_SZ)+1,SEEK_SET,0)<0){
            err_dump("_db_writeidx: writew_lock error");
        }
    }
    /**
     * Position the index file and record the offset.
     * 索引文件定位
     */
    if((db->idxoff=lseek(db->idxfd,offset,whence))==-1){
        err_dump("_db_writeidx: lseek error");
    }
    // 向索引文件中写
    iov[0].iov_base=assiiptrlen;
    iov[0].iov_len=PTR_SZ+IDXLEN_SZ;
    iov[1].iov_base=db->idxbuf;
    iov[1].iov_len=len;
    if(writev(db->idxlen,&iov[0],2)!=PTR_SZ+IDXLEN_SZ+len){
        err_dump("_db_writeidx: wirtev error of index record");
    }
    // 追加解锁
    if(whence==SEEK_END){
        if(un_lock(db->idxfd,((db->nhash+1)*PTR_SZ)+1,SEEK_SET,0)<0){
            err_dump("_db_writeidx: un_lock error");
        }
    }
}

/**
 * Write a chain ptr field somewhere in the index file:
 * the free list,the hash table, or in an index record.
 * 将一个链表指针写到索引文件中
 */
static void _db_writeptr(DB *db,off_t offset,off_t ptrval){
    char asciiptr[PTR_SZ+1];
    // 指针非法
    if(ptrval<0||ptrval>PTR_MAX){
        err_quit("_db_writeptr: invalid ptr: %d",ptrval);
    }
    // 转为ASCII码
    sprintf(asciiptr,"%*ld",PTR_SZ,ptrval);
    // 定位
    if(lseek(db->idxfd,offset,SEEK_SET)==-1){
        err_dump("_db_writeptr: lseek error to ptr field");
    }
    // 写文件
    if(write(db->idxfd,asciiptr,PTR_SZ)!=PTR_SZ){
        err_dump("_db_writeptr: write error of ptr field");
    }
}

/**
 * Store a record in the database. Return 0 if OK, 1 if record
 * exists and DB_INSERT specified, -1 on error.
 * 向数据库中存储一条记录
 */
int db_store(DBHANDLE h,const char *key,const char *data,int flag){
    DB *db=h;
    int rc,keylen,datlen;
    off_t ptrval;
    // 输入的存储标志非法
    if(flag!=DB_INSERT&&flag!=DB_REPLACE&&flag!=DB_STORE){
        errno=EINVAL;
        return(-1);
    }
    // 键长
    keylen=strlen(key);
    // 数据长
    datlen=strlen(data)+1; /* +1 for newline at end */
    // 数据长度非法
    if(datlen<DATLEN_MIN||datlen>DATLEN_MAX){
        err_dump("db_store: invalid data length");
    }
    /**
     * _db_find_and_lock calculates which hash table this new record
     * goes into (db->chainoff), regardless of whether it already 
     * exists or not. The following calls to _db_writeptr change the
     * hash table entry for this chain to point to the new record.
     * The new record is added to the front of the hash chain.
     */
    // 记录没有发现
    if(_db_find_and_lock(db,key,1)<0){ /* record not found */
        // 期望更新
        if(flag==DB_REPLACE){
            rc=-1;
            db->cnt_storerr++;
            errno=ENOENT; /* error,record does not exist */
            goto doreturn; // 返回
        }
        /**
         * _db_find_and_lock locked the hash chain for us; read
         * the chain ptr to the first index record on hash chain.
         * 读散列链上第一项的偏移量
         */
        ptrval=_db_readptr(db,db->chainoff);
        if(_db_findfree(db,keylen,datlen)<0){
            /**
             * Can't find an empty record big enough. Append the
             * new record to the ends of the index and data files.
             */
            _db_writedat(db,data,0,SEEK_END);
            _db_writeidx(db,key,0,SEEK_END,ptrval);
            /**
             * db->idxoff was set by _db_writeidx. The new record goes
             * to the front of the hash chain.
             */
            _db_writeptr(db,db->chainoff,db->idxoff);
            db->cnt_stor1++;
        }else{
            /**
             * Reuse an empty record. _db_findfree removed it from the 
             * free list and set both db->datoff and db->idxoff. Reused
             * record goes to the front of the hash chain.
             */
            _db_writedat(db,data,db->datoff,SEEK_SET);
            _db_writeidx(db,key,db->idxoff,SEEK_SET,ptrval);
            _db_writeptr(db,db->chainoff,db->idxoff);
            db->cnt_stor2++;
        }
    }else{ /* record found */
        // 如果当前是插入
        if(flag==DB_INSERT){ // 报错
            rc=1; /* error,record already in db */
            db->cnt_storerr++;
            goto doreturn;
        }
        /**
         * We are replacing an existing record. We know the new
         * key equals the existing key, but we need to check if
         * the data records are the same size.
         */
        if(datlen!=db->datlen){
            _db_dodelete(db); /* delete the existing record */
            /**
             * Reread the chain ptr in the hash table
             * (it may change with the deletion ).
             */
            ptrval=_db_readptr(db,db->chainoff);
            /**
             * Append new index and data records to end of files.
             */
            _db_writedat(db,data,0,SEEK_END);
            _db_writeidx(db,key,0,SEEK_END,ptrval);
            /**
             * New record goes to the front of the hash chain.
             */
            _db_writeptr(db,db->chainoff,db->idxoff);
            db->cnt_stor3++;
        }else{
            /**
             * Same size data, just replace data record.
             */
            _db_writedat(db,data,db->datoff,SEEK_SET);
            db->cnt_stor4++;
        }
    }
    rc=0; /* OK */
doreturn: /* unlock hash chain locked by _db_find_and_lock */
    if(un_lock(db->idxfd,db->chainoff,SEEK_SET,1)<0){
        err_dump("db_store: un_lock error");
    }
    return(rc);
}

/**
 * Try to find a free index record and accompanying data record
 * of the correct sizes. We're only called by db_store.
 */
static int _db_findfree(DB *db,int keylen,int datlen){
    int rc;
    off_t offset,nextoffset,saveoffset;
    /**
     * Lock the free list.
     */
    if(writew_lock(db->idxfd,FREE_OFF,SEEK_SET,1)<0){
        err_dump("_db_findfree: writew_lock error");
    }
    /**
     * Read the free list pointer.
     */
    saveoffset=FREE_OFF;
    offset=_db_readptr(db,saveoffset);
    while(offset!=0){
        nextoffset=_db_readidx(db,offset);
        if(strlen(db->idxbuf)==keylen&&db->datlen==datlen){
            break; /* found a match */
        }
        saveoffset=offset;
        offset=nextoffset;
    }
    if(offset==0){
        rc=-1; /* no match found */
    }else{
        /**
         * Found a free record with mathing sizes.
         * The index record was read in by _db_readidx above.
         * which sets db->ptrval. Also, saveoffset points to
         * the chain ptr that pointed to this empty record on
         * the free list. We set this chain ptr to db->ptrval,
         * which removes the empty record from the free list.
         */
        _db_writeptr(db,saveoffset,db->ptrval);
        rc=0;
        /**
         * Notice also that _db_readidx set both db->idxoff
         * and db->datoff. This is used by the caller, db_store,
         * to write the new index record and data record.
         */
    }
    /**
     * Unlock the free list.
     */
    if(un_lock(db->idxfd,FREE_OFF,SEEK_SET,1)<0){
        err_dump("_db_findfree: un_lock error");
    }
    // 返回
    return(rc);
}