#include "sysutil.h"
#include "session.h"
#include "tunable.h"
#include "parseconf.h"
#include "common.h"
#include "hash.h"
#include "ftpcodes.h"
#include "ftpproto.h"
#include "clntlmt.h"
#include "test.h"


void handle_sigchld(int signal);

char* listenip = NULL;

int main()
{
    if ( 0 != getuid() )
    {
        fprintf(stderr,"miniftpd should start from root.\n");
        exit(EXIT_FAILURE);
    }

    if ( tunable_listen_address == NULL) 
    {
        //绑定到eth0对应的ip
        listenip = (char*)malloc(sizeof(char) * 128);
        getlocalip(listenip);
    }
    else
        strcpy(listenip,tunable_listen_address);

    parseconf_load_file(CONFFILE);
    printconf();
    hash_test();

    umask(tunable_local_umask);

    signal(SIGCHLD,handle_sigchld);
    int listenfd = tcp_srv(listenip,5188);
    
    pid_t pid;
    int conn;
    

    clntlmt_init();

    //daemon(0,0);

    while (1)
    {
        struct sockaddr_in addr;
        conn = accept_timeout(listenfd,&addr,tunable_connect_timeout);
        if ( -1 == conn )
            ERR_EXIT("connect error");

        
        session_t sessioninf;
        init_session(&sessioninf,conn);

        int lmt_fd[2];
        if ( -1 == socketpair(AF_UNIX,SOCK_STREAM,0,lmt_fd) )
            ERR_EXIT("socketpair");

        pid = fork();
        if (-1 == pid)
            ERR_EXIT("fork");


        if (0 == pid)
        {
            signal(SIGCHLD,SIG_IGN);
            close(listenfd);
            close(lmt_fd[0]);
            char cmd = '\0';
            readn(lmt_fd[1],&cmd,1);
            if ( cmd == 'r' )
            {
                close(lmt_fd[1]);
            }
            else if ( cmd == 'i' )
            {
                close(lmt_fd[1]);
                ftp_reply(&sessioninf,FTP_TOO_MANY_USERS,"There are too many connections from your internet address");
                exit(EXIT_FAILURE);    
            }
            else 
            {
                close(lmt_fd[1]);
                ftp_reply(&sessioninf,FTP_TOO_MANY_USERS,"There are too many connected users, please try later");
                exit(EXIT_FAILURE);    
            }
            begin_session(&sessioninf);
            break;
        }
        if (pid > 0)
        {
            close(lmt_fd[1]);
            //检查是否满足limit限制
            clntlmt_add(pid,addr.sin_addr.s_addr);
            int ret = clntlmt_check(addr.sin_addr.s_addr);
            if ( 0 == ret )
            {
                writen(lmt_fd[0],"r",1);
            }
            if ( -1 == ret )
            {
            }
            else if ( -2 == ret )
                printf("max clients total.\n");    
            close(conn);
            close(lmt_fd[0]);
        }
    }
        

    free(listenip);

    return 0;
}



unsigned int hash_fun(void* key,int hashtable_size)
{
    unsigned int hash = 0;
    char* str = key;
    while(*str)
    {
        hash += (hash << 5) + (*str++);
    }

    return ( hash & 0x7fffffff ) % hashtable_size;
}

/*
 * 接收子进程退出消息，维护clients变量和每ip客户端数目
 */
void handle_sigchld(int signal)
{
    pid_t pid;
    while ( ( pid=waitpid( -1,NULL,WNOHANG)) > 0 )
    {
        clntlmt_del(pid);
    }
}
