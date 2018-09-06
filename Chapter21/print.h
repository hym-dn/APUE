#ifndef _PRINT_H
#define _PRINT_H

/*
 * Print server header file.
 * 包含常用的常数、数据结构定义以及工具例程声明的头文件
 */

// 依赖头文件
#include <sys/socket.h>
#include <arpa/inet.h>
#if defined(BSD) || defined(MACOS)
#include <netinet/in.h>
#endif
#include <netdb.h>
#include <errno.h>

// 定义实现所需的文件和目录
#define CONFIG_FILE    "/etc/printer.conf"
#define SPOOLDIR       "/var/spool/printer"
#define JOBFILE        "jobno"
#define DATADIR        "data"
#define REQDIR         "reqs"

// 定义限制和常数
#define FILENMSZ        64
#define FILEPERM        (S_IRUSR|S_IWUSR) // 创建提交打印文件副本时使用的权限
#define USERNM_MAX      64
#define JOBNM_MAX       256
#define MSGLEN_MAX      512

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX   256
#endif

#define IPP_PORT        631 // IPP使用端口
#define QLEN            10
#define IBUFSZ          512	/* IPP header buffer size */
#define HBUFSZ          512	/* HTTP header buffer size */
#define IOBUFSZ         8192	/* data buffer size */

// 如果当前平台不定义ETIME错误，则定义之
#ifndef ETIME
#define ETIME ETIMEDOUT
#endif

// 声明公共例程
extern int getaddrlist(const char *, const char *,
  struct addrinfo **);
extern char *get_printserver(void);
extern struct addrinfo *get_printaddr(void);
extern ssize_t tread(int, void *, size_t, unsigned int);
extern ssize_t treadn(int, void *, size_t, unsigned int);
extern int connect_retry(int, const struct sockaddr *, socklen_t);
extern int initserver(int, struct sockaddr *, socklen_t, int);

/*
 * Structure describing a print request.
 * 打印请求洗协议
 */
struct printreq {
	long size;					/* size in bytes */
	long flags;					/* see below */
	char usernm[USERNM_MAX];	/* user's name */
	char jobnm[JOBNM_MAX];		/* job's name */
};

/*
 * Request flags.
 */
#define PR_TEXT		0x01	/* treat file as plain text */

/*
 * The response from the spooling daemon to the print command.
 * 打印响应协议
 */
struct printresp {
	long retcode;				/* 0=success, !0=error code */
	long jobid;					/* job ID */
	char msg[MSGLEN_MAX];		/* error message */
};

#endif /* _PRINT_H */
