#ifndef _CLNTLMT_H_
#define _CLNTLMT_H_

#include "common.h"
/*
 * 本模块是用于客户端连接限制，包括总连接限制和每ip限制
 */

int clntlmt_init();     //初始化hash表
int clntlmt_destroy();  //销毁hash表，释放内存
void clntlmt_add(pid_t pid,unsigned int ip);    //添加记录
void clntlmt_del(pid_t pid);    //删除记录
int clntlmt_check();            //检查是超过限制，-1表示超过每ip最大连接数，-2表示超过总连接数

#endif
