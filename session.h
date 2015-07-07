#ifndef _SESSION_H_
#define _SESSION_H_

#include "common.h"

typedef struct session
{
    int nobodyfd;   //nobody进程通讯套接字
    int protofd;    //proto进程通讯套接字
    int ctrlfd;     //与控制进程相连的客户端套接字
    int datafd;     //数据连接套接字

    int bASCII;
    int bappe;

    char cmdline[MAXCMDLINE];
    char cmd[MAXCMDLINE];
    char arg[MAXCMDLINE];

    struct sockaddr_in* port_addr;
    int pasv_lsfd;

    char* rn_filepath;

    long long rest_pos;

    unsigned int ts_sec;
    unsigned int ts_usec;

    int burg;



    unsigned int ip;
}session_t;

int begin_session(session_t* sessioninf);
void clear_session(session_t* sessioninf);
void init_session(session_t* sessioninf,int fd);
void clear_transfer(session_t* ssinf);
int has_port(session_t *sess);
int has_pasv(session_t *sess);

int ratelim_init(session_t* sess);
int ratelim_do(session_t* sess,unsigned int rate,int nbytes);


#endif
