#include "ftpipc.h"
#include "sysutil.h"

int ftpipc_send_int(int fd,const int val);
int ftpipc_recv_int(int fd);

int ftpipc_init(session_t* ssinf)
{
    int fd[2];
    if (-1 == socketpair(AF_UNIX,SOCK_STREAM,0,fd))
        ERR_EXIT("socketpair error");

    ssinf->protofd = fd[0];
    ssinf->nobodyfd = fd[1];

    return 0;
}
void ftpipc_close(session_t* ssinf)
{
    if ( ssinf->protofd != -1 )
        close(ssinf->protofd);
    if ( ssinf->nobodyfd != -1 )
        close(ssinf->nobodyfd);
}
void ftpipc_setnobody(session_t* ssinf)
{
    if ( ssinf->protofd != -1 )
        close(ssinf->protofd);
}
void ftpipc_setproto(session_t* ssinf)
{
    if ( ssinf->nobodyfd != -1 )
        close(ssinf->nobodyfd);
}


int ftpipc_send_cmd(int fd,char cmd)
{
    return writen(fd,&cmd,sizeof(cmd));
}
int ftpipc_recv_cmd(int fd)
{
    char cmd;
    int ret;
    ret = readn(fd,&cmd,sizeof(cmd));
    if ( sizeof(cmd) == ret ) 
        return cmd;
    return ret;
}

int ftpipc_send_ret(int fd,int ret)
{
    return writen(fd,&ret,sizeof(ret));
}
int ftpipc_recv_ret(int fd)
{
    int ret;
    if ( -1 == readn(fd,&ret,sizeof(ret)))
        return -1;
    return ret;
}

int ftpipc_send_data(int fd,const void* str,int bufsize)
{
    if ( -1 == ftpipc_send_int(fd,bufsize) )
        return -1;

    return writen(fd,str,bufsize);
}
int ftpipc_recv_data(int fd,void* str,int maxsize)
{
    int bufsize = ftpipc_recv_int(fd);
    if ( bufsize > maxsize )
        return -1;

    if ( bufsize != readn(fd,str,bufsize) )
        return -1;

    return 0;
}

int ftpipc_send_int(int fd,int val)
{
    int bufsize = sizeof(int);
    if ( bufsize != writen(fd,&val,bufsize) )
        return -1;
    return 0;
}
int ftpipc_recv_int(int fd)
{
    int ret;
    if ( sizeof(int) != readn(fd,&ret,sizeof(int)))
        return -1;
    return ret;
}
int ftpipc_send_fd(int fd,int datafd)
{
    send_fd(fd,datafd); 
    return 0;
}
int ftpipc_recv_fd(int fd)
{
    return recv_fd(fd);
}
