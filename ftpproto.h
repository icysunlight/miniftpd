#ifndef _FTPPROTO_H_
#define _FTPPROTO_H_

#include "session.h"

int handle_ftp(session_t* sessioninf);
int ftp_reply(session_t* ssinf,int status,const char* text);
extern session_t* psession;

#endif
