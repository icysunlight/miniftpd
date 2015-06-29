#ifndef _SYSUTIL_H_
#define _SYSUTIL_H_


#include "common.h"

void printaddr(struct sockaddr_in* addr);

int file_unlock(int filefd);
int file_rdlock(int filefd);
int file_wrlock(int filefd);

int activate_oobinline(int fd);
int activate_sigurg(int fd);


int tcp_srv(const char* ip,const unsigned short port);
int tcp_clnt(const char* host,const unsigned short port);

int getlocalip(char* ip);

void activate_nonblock(int fd);
void deactivate_nonblock(int fd);

int read_timeout(int fd, unsigned int wait_seconds);
int write_timeout(int fd, unsigned int wait_seconds);
int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds);
int connect_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds);

ssize_t writen(int fd, const void* buf, size_t count);
ssize_t readn(int fd, void* buf, size_t count);
ssize_t read_line(int sock_fd, void* buf, size_t max_len);
ssize_t recv_peek (int sock_fd, void *buf, size_t len);//peek data
ssize_t readline(int sock_fd, void* buf, size_t max_len);

void send_fd(int sock_fd, int fd);
int recv_fd(const int sock_fd);

#endif
