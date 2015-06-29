#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define max(a,b) (((a) > (b)) ? (a) : (b))

#define ERR_EXIT(m) \
	do \
	{ \
		perror(m); \
		exit(EXIT_FAILURE); \
	}while(0)

ssize_t readn(int fd, void *buf, size_t count)
{
	size_t nleft = count;	
	char* bufp = buf;
	int nread = 0;
	while (nleft > 0)
	{
		nread = read(fd,bufp,nleft);
		if (0 == nread)
			return count - nleft;
		if (-1 == nread)
		{
			if (EINTR == errno)
                continue;
			return -1;
		}
		bufp += nread;
		nleft -= nread;
	}
	return count;
}

ssize_t writen(int fd, void *buf, size_t count)
{
	size_t nleft = count;	
	char* bufp = buf;
	int nwrite = 0;
	while (nleft > 0)
	{
		nwrite = write(fd,bufp,nleft);
		if (0 == nwrite)
			continue;
		if (-1 == nwrite)
		{
			if (EINTR == errno)
				continue;
			return -1;
		}
		bufp += nwrite;
		nleft -= nwrite;
	}
	return count;
}

void echo_clnt(int clntfd)
{
	char buf[1024];
	memset(buf,0,1024 * sizeof(char));

	fd_set readfds;
	FD_ZERO(&readfds);
	int stdinfd = fileno(stdin);
	int maxfd = max(stdinfd,clntfd);
	int stdineof = 0;

	while (1)
	{
		FD_SET(clntfd,&readfds);
		if (0 == stdineof)
		{
			FD_SET(stdinfd,&readfds);
		}

		int ret;
		do
		{
			ret = select(maxfd + 1,&readfds,NULL,NULL,NULL);
		}while(-1 == ret && EINTR == errno);

		if (-1 == ret)
			ERR_EXIT("select error");
		
		if (FD_ISSET(clntfd,&readfds))
		{
			int ret = read(clntfd,buf,1024);	
			if (-1 == ret)
				ERR_EXIT("read error");
			if (0 == ret)
			{
				printf("server close\n");
				close(clntfd);
				return;
			}
			fputs(buf,stdout);
			memset(buf,0,1024 * sizeof(char));
		}
		if (FD_ISSET(stdinfd,&readfds))
		{
			if (NULL == fgets(buf,1024,stdin))
			{
				shutdown(clntfd,SHUT_WR);
				stdineof = 1;
			}
			else
			{
				write(clntfd,buf,strlen(buf));
				memset(buf,0,1024 * sizeof(char));
			}
		}

		FD_ZERO(&readfds);
	}
}
int deactive_nonblock(int fd)
{
	unsigned int flags = fcntl(fd,F_GETFL,0);
	if (-1 == flags)
		return -1;
	return fcntl(fd,F_SETFL,flags & ~O_NONBLOCK);
}
int active_nonblock(int fd)
{
	unsigned int flags = fcntl(fd,F_GETFL,0);
	if (-1 == flags)
		return -1;
	return fcntl(fd,F_SETFL,flags | O_NONBLOCK);
}
int connect_timeout(int clntfd,struct sockaddr_in* clntaddr,int timelimit)
{
	if (timelimit > 0 && 0 != active_nonblock(clntfd))
		return -1;
	
	socklen_t addrlen = sizeof(*clntaddr);
	int ret = connect(clntfd,(struct sockaddr*)clntaddr,addrlen);

	if (-1 == ret && EINPROGRESS == errno)
	{
		fd_set wrfds;
		FD_ZERO(&wrfds);
		FD_SET(clntfd,&wrfds);
		struct timeval timeout = {timelimit,0};
		do
		{
			ret = select(clntfd + 1,NULL,&wrfds,NULL,&timeout);
		}while(-1 == ret && EINTR == errno);

		if (-1 == ret)
			return -1;
		else if (0 == ret)
		{
			errno = ETIMEDOUT;
			ret = -1;
		}
		else if (1 == ret)
		{
			int err = -1;
			socklen_t socklen = sizeof(int);
			int sockopterr = getsockopt(clntfd,SOL_SOCKET,SO_ERROR,&err,&socklen);
			if (-1 == sockopterr)
				return -1;
			if (0 == err)
				ret = 0;
			else
			{
				ret = -1;
				errno = err;
			}
		}
	}

	if (timelimit > 0 && 0 != active_nonblock(clntfd))
		return -1;

	return ret;
}
int main(int argc,const char* argv[])
{
	int clntfd;

	if (-1 == (clntfd = socket(PF_INET,SOCK_STREAM,0)))
		ERR_EXIT("socket:");
	
	struct sockaddr_in sveraddr;
	memset(&sveraddr,0,sizeof(sveraddr));
	sveraddr.sin_family = PF_INET;
	sveraddr.sin_port = htons(atoi(argv[2])); 
	sveraddr.sin_addr.s_addr = inet_addr(argv[1]);
	/*
	if ( -1 == connect(clntfd,(struct sockaddr*)&sveraddr,sizeof(sveraddr)))
		ERR_EXIT("connect");
	*/
	
	int ret = 0;
	do
	{
		ret = connect_timeout(clntfd,&sveraddr,1);
	}while(-1 == ret && ETIMEDOUT == errno);

	if (-1 == ret)
		ERR_EXIT("connect");

    //printf("connect success\n");

	echo_clnt(clntfd);

	return 0;
}
