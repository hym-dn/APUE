/*
 * The client command for printing documents.  Opens the file
 * and sends it to the printer spooling daemon.  Usage:
 * 	print [-t] filename
 * 用于打印文件的命令行程序C源文件
 */

#include "apue.h"
#include "print.h"
#include <fcntl.h>
#include <pwd.h>

/*
 * Needed for logging funtions.
 * 通过这个整数能够使用函数库中的日志函数，如果
 * 该整数设置为非零值，错误消息将被送到标准错误
 * 输出来取代日志文件。
 */
int log_to_stderr = 1;

void submit_file(int, int, const char *, size_t, int);


int
main(int argc, char *argv[])
{
	int				fd, sockfd, err, text, c;
	struct stat		sbuf;
	char			*host;
	struct addrinfo	*ailist, *aip;
	// 分析命令行参数，支持-t选项
	err = 0;
	text = 0;
	while ((c = getopt(argc, argv, "t")) != -1) {
		switch (c) {
		case 't':
			text = 1;
			break;

		case '?':
			err = 1;
			break;
		}
	}
	// 命令行参数解析失败
	if (err || (optind != argc - 1))
		err_quit("usage: print [-t] filename");
	// 打开指定文件
	if ((fd = open(argv[optind], O_RDONLY)) < 0)
		err_sys("print: can't open %s", argv[1]);
	// 获取文件属性
	if (fstat(fd, &sbuf) < 0)
		err_sys("print: can't stat %s", argv[1]);
	// 检验是否为常规文件
	if (!S_ISREG(sbuf.st_mode))
		err_quit("print: %s must be a regular file\n", argv[1]);
	/*
	 * Get the hostname of the host acting as the print server.
	 */
	// 取得打印假脱机守护进程的名字
	if ((host = get_printserver()) == NULL)
		err_quit("print: no print server defined");
	// 将主机名字映射为网络地址（注意：服务名称为"print"）
	if ((err = getaddrlist(host, "print", &ailist)) != 0)
		err_quit("print: getaddrinfo error: %s", gai_strerror(err));
	// 地址链表
	for (aip = ailist; aip != NULL; aip = aip->ai_next) {
		// 创建socket
		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // 失败
			err = errno;
		}
		// 尝试连接服务器
		else if (connect_retry(sockfd, aip->ai_addr,
		  aip->ai_addrlen) < 0) { // 失败
			err = errno;
		} 
		// 连接服务器成功
		else {
			// 发送文件到守护进程
			submit_file(fd, sockfd, argv[1], sbuf.st_size, text);
			// 退出程序
			exit(0);
		}
	}
	// 连接假脱机守护进程失败
	errno = err;
	err_ret("print: can't contact %s", host);
	// 错误退出
	exit(1);
}

/*
 * Send a file to the printer daemon.
 * 发送打印请求到守护进程程序并读取相应消息
 */
void
submit_file(int fd, int sockfd, const char *fname, size_t nbytes,
            int text)
{
	int					nr, nw, len;
	struct passwd		*pwd;
	struct printreq		req;
	struct printresp	res;
	char				buf[IOBUFSZ];
	/*
	 * First build the header.
	 */
	// 获取当前进程有效用户口令
	if ((pwd = getpwuid(geteuid())) == NULL)
		strcpy(req.usernm, "unknown"); // 失败
	else
		strcpy(req.usernm, pwd->pw_name); // 成功，记录用户名称
	// 转为网络字节序
	req.size = htonl(nbytes);
	if (text) // 是否纯文本格式打印
		req.flags = htonl(PR_TEXT);
	else
		req.flags = 0;
	// 如果文件名超出了报文所能容纳的长度，则将名字的开头部分截去，并带入省略号
	if ((len = strlen(fname)) >= JOBNM_MAX) {
		/*
		 * Truncate the filename (+-5 accounts for the leading
		 * four characters and the terminating null).
		 */
		strcpy(req.jobnm, "... ");
		strncat(req.jobnm, &fname[len-JOBNM_MAX+5], JOBNM_MAX-5);
	} 
	// 存储报文文件名
	else {
		strcpy(req.jobnm, fname);
	}
	/*
	 * Send the header to the server.
	 * 发送【首部】到服务器守护进程
	 */
	nw = writen(sockfd, &req, sizeof(struct printreq));
	if (nw != sizeof(struct printreq)) { // 发送失败
		if (nw < 0)
			err_sys("can't write to print server");
		else
			err_quit("short write (%d/%d) to print server",
			  nw, sizeof(struct printreq));
	}
	/*
	 * Now send the file.
	 * 读取文件内容，并将文件发送给守护进程
	 */
	while ((nr = read(fd, buf, IOBUFSZ)) != 0) {
		nw = writen(sockfd, buf, nr);
		if (nw != nr) { // 发送失败
			if (nw < 0)
				err_sys("can't write to print server");
			else
				err_quit("short write (%d/%d) to print server",
				  nw, nr);
		}
	}
	/*
	 * Read the response.
	 * 阻塞读取应答
	 */
	if ((nr = readn(sockfd, &res, sizeof(struct printresp))) !=
	  sizeof(struct printresp))
		err_sys("can't read response from server");
	if (res.retcode != 0) {
		printf("rejected: %s\n", res.msg);
		exit(1);
	} else {
		printf("job ID %ld\n", ntohl(res.jobid));
	}
	// 退出
	exit(0);
}
