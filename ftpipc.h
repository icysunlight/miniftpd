#ifndef _FTPIPC_H_
#define _FTPIPC_H_

#include "session.h"

#define FTPIPC_GET_DATA_FD 1

#define FTPIPC_RET_OK 1
#define FTPIPC_RET_BAD 2

int ftpipc_init(session_t* ssinf);
void ftpipc_close(session_t* ssinf);
void ftpipc_setnobody(session_t* ssinf);
void ftpipc_setproto(session_t* ssinf);



int ftpipc_send_cmd(int fd,char cmd);
int ftpipc_recv_cmd(int fd);

int ftpipc_send_data(int fd,const void* str,int datasize);
int ftpipc_recv_data(int fd,void* str,int maxsize);

int ftpipc_send_ret(int fd,int ret);
int ftpipc_recv_ret(int fd);

int ftpipc_send_fd(int fd,int datafd);
int ftpipc_recv_fd(int fd);

#endif
