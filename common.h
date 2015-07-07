#ifndef _COMMON_H
#define _COMMON_H



#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <shadow.h>
#include <pwd.h>
#include <dirent.h>
#include <time.h>
#include <signal.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sys/sendfile.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/capability.h>
#include <crypt.h>
#include <pwd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define MAXCMDLINE 1024
#define CONFFILE "./miniftpd.conf"

extern char* listenip;

#define ERR_EXIT(m) \
        do \
        { \
            perror(m); \
            exit(EXIT_FAILURE); \
        }while(0)

#endif
