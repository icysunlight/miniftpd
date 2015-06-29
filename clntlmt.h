#ifndef _CLNTLMT_H_
#define _CLNTLMT_H_

#include "common.h"
/*
 * 本模块是用于客户端连接限制，包括总连接限制和每ip限制
 */
int clntlmt_init(int totalclnts,int peripclnts);
int clntlmt_destroy();
void clntlmt_add(pid_t pid,unsigned int ip);
void clntlmt_del(pid_t pid);

#endif
