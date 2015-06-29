#include "sysutil.h"
#include "common.h"

void printaddr(struct sockaddr_in* addr)
{
    printf("ip:%s\tport:%d\n",inet_ntoa(addr->sin_addr),ntohs(addr->sin_port)); 
}

int file_rdlock(int filefd)
{
    struct flock lk;
    lk.l_type = F_RDLCK;
    lk.l_whence = SEEK_SET;
    lk.l_start = 0;
    lk.l_len = 0;

    return fcntl(filefd,F_SETLK,&lk);
}

int file_wrlock(int filefd)
{
    struct flock lk;
    lk.l_type = F_WRLCK;
    lk.l_whence = SEEK_SET;
    lk.l_start = 0;
    lk.l_len = 0;

    return fcntl(filefd,F_SETLK,&lk);
}
int file_unlock(int filefd)
{
    struct flock lk;
    lk.l_type = F_UNLCK;
    lk.l_whence = SEEK_SET;
    lk.l_start = 0;
    lk.l_len = 0;

    return fcntl(filefd,F_SETLK,&lk);
}
int activate_oobinline(int fd)
{
    int opt = 1;
    return setsockopt(fd,SOL_SOCKET,SO_OOBINLINE,&opt,sizeof(opt));
}
int activate_sigurg(int fd)
{
    return fcntl(fd,F_SETOWN,getpid());
}

int tcp_srv(const char* host,const unsigned short port)
{
    int fd = socket(AF_INET,SOCK_STREAM,0);
    if (-1 == fd)
        ERR_EXIT("socket error");

    int on = 1;
    if (-1 == setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(const char*)&on,sizeof(on)))
        ERR_EXIT("setsockopt error");

    struct sockaddr_in srvaddr;
    memset(&srvaddr,0,sizeof(srvaddr));
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_port = htons(port);
    if (NULL != host)
    {
        if (0 == inet_aton(host,&srvaddr.sin_addr))
            ERR_EXIT("inet_aton");
    }
    else
        srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (-1 == bind(fd,(struct sockaddr*)&srvaddr,sizeof(srvaddr)))
        ERR_EXIT("bind error");

    if (-1 == listen(fd,10))
        ERR_EXIT("listen error");

    return fd;

}

int tcp_clnt(const char* host,const unsigned short port)
{
    int fd = socket(PF_INET,SOCK_STREAM,0);
    if (-1 == fd)
        ERR_EXIT("socket error");

    int on = 1;
    if (-1 == setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(const char*)&on,sizeof(on)))
        ERR_EXIT("setsockopt error");
    
    if ( port > 0 )
    {
        struct sockaddr_in srvaddr;
        memset(&srvaddr,0,sizeof(srvaddr));
        srvaddr.sin_family = AF_INET;
        srvaddr.sin_port = htons(port);
        if (0 == inet_aton(host,&srvaddr.sin_addr))
            ERR_EXIT("inet_aton");
        if (-1 == bind(fd,(struct sockaddr*)&srvaddr,sizeof(srvaddr)))
            ERR_EXIT("bind error");
    }

    return fd;
}
/*
int getlocalip(char *ip)
{
    char host[100] = {0};
    if (gethostname(host, sizeof(host)) < 0)
        return -1;
    struct hostent *hp;
    if ((hp = gethostbyname(host)) == NULL)
        return -1;

    strcpy(ip, inet_ntoa(*((struct in_addr*)hp->h_addr_list)));
    return 0;
}
*/
int getlocalip(char *ip)
{
    int sockfd; 
    if(-1 == (sockfd = socket(PF_INET, SOCK_STREAM, 0)))
    {
        perror( "socket" );
        return -1;
    }
    struct ifreq req;
    struct sockaddr_in *host;
    bzero(&req, sizeof(struct ifreq));
    strcpy(req.ifr_name, "eth0"); 
    ioctl(sockfd, SIOCGIFADDR, &req);
    host = (struct sockaddr_in*)&req.ifr_addr;
    strcpy(ip, inet_ntoa(host->sin_addr));
    close(sockfd);
    return 1;
}

/**
 * activate_noblock - ??I/O??????
 * @fd: ?????
 */
void activate_nonblock(int fd)
{
    int ret;
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1)
        ERR_EXIT("fcntl");

    flags |= O_NONBLOCK;
    ret = fcntl(fd, F_SETFL, flags);
    if (ret == -1)
        ERR_EXIT("fcntl");
}

/**
 * deactivate_nonblock - ??I/O?????
 * @fd: ?????
 */
void deactivate_nonblock(int fd)
{
    int ret;
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1)
        ERR_EXIT("fcntl");

    flags &= ~O_NONBLOCK;
    ret = fcntl(fd, F_SETFL, flags);
    if (ret == -1)
        ERR_EXIT("fcntl");
}

/**
 * read_timeout - ?????????????
 * @fd: ?????
 * @wait_seconds: ??????????0???????
 * ?????????0?????-1?????-1??errno = ETIMEDOUT
 */
int read_timeout(int fd, unsigned int wait_seconds)
{
    int ret = 0;
    if (wait_seconds > 0)
    {
        fd_set read_fdset;
        struct timeval timeout;

        FD_ZERO(&read_fdset);
        FD_SET(fd, &read_fdset);

        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;
        do
        {
            ret = select(fd + 1, &read_fdset, NULL, NULL, &timeout);
        } while (ret < 0 && errno == EINTR);

        if (ret == 0)
        {
            ret = -1;
            errno = ETIMEDOUT;
        }
        else if (ret == 1)
            ret = 0;
    }

    return ret;
}

/**
 * write_timeout - ?????????????
 * @fd: ?????
 * @wait_seconds: ??????????0???????
 * ?????????0?????-1?????-1??errno = ETIMEDOUT
 */
int write_timeout(int fd, unsigned int wait_seconds)
{
    int ret = 0;
    if (wait_seconds > 0)
    {
        fd_set write_fdset;
        struct timeval timeout;

        FD_ZERO(&write_fdset);
        FD_SET(fd, &write_fdset);

        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;
        do
        {
            ret = select(fd + 1, NULL, NULL, &write_fdset, &timeout);
        } while (ret < 0 && errno == EINTR);

        if (ret == 0)
        {
            ret = -1;
            errno = ETIMEDOUT;
        }
        else if (ret == 1)
            ret = 0;
    }

    return ret;
}

/**
 * accept_timeout - ????accept
 * @fd: ???
 * @addr: ???????????
 * @wait_seconds: ??????????0??????
 * ????????????????????-1??errno = ETIMEDOUT
 */
int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
    int ret;
    socklen_t addrlen = sizeof(struct sockaddr_in);

    if (wait_seconds > 0)
    {
        fd_set accept_fdset;
        struct timeval timeout;
        FD_ZERO(&accept_fdset);
        FD_SET(fd, &accept_fdset);
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;
        do
        {
            ret = select(fd + 1, &accept_fdset, NULL, NULL, &timeout);
        } while (ret < 0 && errno == EINTR);
        if (ret == -1)
            return -1;
        else if (ret == 0)
        {
            errno = ETIMEDOUT;
            return -1;
        }
    }

    if (addr != NULL)
        ret = accept(fd, (struct sockaddr*)addr, &addrlen);
    else
        ret = accept(fd, NULL, NULL);
/*  if (ret == -1)
        ERR_EXIT("accept");
        */

    return ret;
}

/**
 * connect_timeout - connect
 * @fd: ???
 * @addr: ????????
 * @wait_seconds: ??????????0??????
 * ?????????0?????-1?????-1??errno = ETIMEDOUT
 */
int connect_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
    int ret;
    socklen_t addrlen = sizeof(struct sockaddr_in);

    if (wait_seconds > 0)
        activate_nonblock(fd);

    ret = connect(fd, (struct sockaddr*)addr, addrlen);
    if (ret < 0 && errno == EINPROGRESS)
    {
        //printf("AAAAA\n");
        fd_set connect_fdset;
        struct timeval timeout;
        FD_ZERO(&connect_fdset);
        FD_SET(fd, &connect_fdset);
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;
        do
        {
            /* ????????????? */
            ret = select(fd + 1, NULL, &connect_fdset, NULL, &timeout);
        } while (ret < 0 && errno == EINTR);
        if (ret == 0)
        {
            ret = -1;
            errno = ETIMEDOUT;
        }
        else if (ret < 0)
            return -1;
        else if (ret == 1)
        {
            //printf("BBBBB\n");
            /* ret???1??????????????????????????????*/
            /* ???????????errno???????????getsockopt???? */
            int err;
            socklen_t socklen = sizeof(err);
            int sockoptret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &socklen);
            if (sockoptret == -1)
            {
                return -1;
            }
            if (err == 0)
            {
                //printf("DDDDDDD\n");
                ret = 0;
            }
            else
            {
                //printf("CCCCCC\n");
                errno = err;
                ret = -1;
            }
        }
    }
    if (wait_seconds > 0)
    {
        deactivate_nonblock(fd);
    }
    return ret;
}

/**
 * readn - ???????
 * @fd: ?????
 * @buf: ?????
 * @count: ???????
 * ????count?????-1???EOF??<count
 */
ssize_t readn(int fd, void *buf, size_t count)
{
    size_t nleft = count;
    ssize_t nread;
    char *bufp = (char*)buf;

    while (nleft > 0)
    {
        if ((nread = read(fd, bufp, nleft)) < 0)
        {
            if (errno == EINTR)
                continue;
            return -1;
        }
        else if (nread == 0)
            return count - nleft;

        bufp += nread;
        nleft -= nread;
    }

    return count;
}

/**
 * writen - ???????
 * @fd: ?????
 * @buf: ?????
 * @count: ???????
 * ????count?????-1
 */
ssize_t writen(int fd, const void *buf, size_t count)
{
    size_t nleft = count;
    ssize_t nwritten;
    char *bufp = (char*)buf;

    while (nleft > 0)
    {
        if ((nwritten = write(fd, bufp, nleft)) < 0)
        {
            if (errno == EINTR)
                continue;
            return -1;
        }
        else if (nwritten == 0)
            continue;

        bufp += nwritten;
        nleft -= nwritten;
    }

    return count;
}

/**
 * recv_peek - ???????????????????
 * @sockfd: ???
 * @buf: ?????
 * @len: ??
 * ????>=0?????-1
 */
ssize_t recv_peek(int sockfd, void *buf, size_t len)
{
    while (1)
    {
        int ret = recv(sockfd, buf, len, MSG_PEEK);
        if (ret == -1 && errno == EINTR)
            continue;
        return ret;
    }
}

/**
 * readline - ??????
 * @sockfd: ???
 * @buf: ?????
 * @maxline: ??????
 * ????>=0?????-1
 */
ssize_t readline(int sockfd, void *buf, size_t maxline)
{
    int ret;
    int nread;
    char *bufp = buf;
    int nleft = maxline;
    while (1)
    {
        ret = recv_peek(sockfd, bufp, nleft);
        if (ret < 0)
            return ret;
        else if (ret == 0)
            return ret;

        nread = ret;
        int i;
        for (i=0; i<nread; i++)
        {
            if (bufp[i] == '\n')
            {
                ret = readn(sockfd, bufp, i+1);
                if (ret != i+1)
                    exit(EXIT_FAILURE);

                return ret;
            }
        }

        if (nread > nleft)
            exit(EXIT_FAILURE);

        nleft -= nread;
        ret = readn(sockfd, bufp, nread);
        if (ret != nread)
            exit(EXIT_FAILURE);

        bufp += nread;
    }

    return -1;
}

void send_fd(int sock_fd, int fd)
{
    int ret;
    struct msghdr msg;
    struct cmsghdr *p_cmsg;
    struct iovec vec;
    char cmsgbuf[CMSG_SPACE(sizeof(fd))];
    int *p_fds;
    char sendchar = 0;
    msg.msg_control = cmsgbuf;
    msg.msg_controllen = sizeof(cmsgbuf);
    p_cmsg = CMSG_FIRSTHDR(&msg);
    p_cmsg->cmsg_level = SOL_SOCKET;
    p_cmsg->cmsg_type = SCM_RIGHTS;
    p_cmsg->cmsg_len = CMSG_LEN(sizeof(fd));
    p_fds = (int*)CMSG_DATA(p_cmsg);
    *p_fds = fd;

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &vec;
    msg.msg_iovlen = 1;
    msg.msg_flags = 0;

    vec.iov_base = &sendchar;
    vec.iov_len = sizeof(sendchar);
    ret = sendmsg(sock_fd, &msg, 0);
    if (ret != 1)
        ERR_EXIT("sendmsg");
}

int recv_fd(const int sock_fd)
{
    int ret;
    struct msghdr msg;
    char recvchar;
    struct iovec vec;
    int recv_fd;
    char cmsgbuf[CMSG_SPACE(sizeof(recv_fd))];
    struct cmsghdr *p_cmsg;
    int *p_fd;
    vec.iov_base = &recvchar;
    vec.iov_len = sizeof(recvchar);
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &vec;
    msg.msg_iovlen = 1;
    msg.msg_control = cmsgbuf;
    msg.msg_controllen = sizeof(cmsgbuf);
    msg.msg_flags = 0;

    p_fd = (int*)CMSG_DATA(CMSG_FIRSTHDR(&msg));
    *p_fd = -1;
    ret = recvmsg(sock_fd, &msg, 0);
    if (ret != 1)
        ERR_EXIT("recvmsg");

    p_cmsg = CMSG_FIRSTHDR(&msg);
    if (p_cmsg == NULL)
        ERR_EXIT("no passed fd");


    p_fd = (int*)CMSG_DATA(p_cmsg);
    recv_fd = *p_fd;
    if (recv_fd == -1)
        ERR_EXIT("no passed fd");

    return recv_fd;
}

