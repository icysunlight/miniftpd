#include "sysutil.h"
#include "session.h"
#include "tunable.h"
#include "parseconf.h"
#include "common.h"
#include "hash.h"
#include "ftpcodes.h"
#include "ftpproto.h"



//客户端连接数限制
static int clients;
void check_clients(session_t* sess);
void handle_sigchld(int signal);
//每ip连接数限制
hashtable_t* hash_ip_clients;
hashtable_t* hash_pid_ip;
int get_clients_num(unsigned int ip);
int add_client(unsigned int ip);
int del_client(pid_t pid);
unsigned int hash_fun_limit(void* key,int hashtable_size);

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

    clients = 0;
    umask(tunable_local_umask);

    hash_ip_clients = hashtable_init(2 * tunable_max_clients,hash_fun_limit);
    hash_pid_ip = hashtable_init(2 * tunable_max_clients,hash_fun_limit);
    signal(SIGCHLD,handle_sigchld);
    int listenfd = tcp_srv(listenip,5188);
    
    pid_t pid;
    int conn;

    //daemon(0,0);

    while (1)
    {
        struct sockaddr_in addr;
        conn = accept_timeout(listenfd,&addr,tunable_connect_timeout);
        if ( -1 == conn )
            ERR_EXIT("connect error");

        
        session_t sessioninf;
        init_session(&sessioninf,conn);
        sessioninf.ip = addr.sin_addr.s_addr;
        sessioninf.clients = ++clients;
        sessioninf.ip_clients = add_client(addr.sin_addr.s_addr);

        pid = fork();
        if (-1 == pid)
            ERR_EXIT("fork");


        if (0 == pid)
        {
            signal(SIGCHLD,SIG_IGN);
            close(listenfd);
            check_clients(&sessioninf);
            begin_session(&sessioninf);
            break;
        }
        if (pid > 0)
        {
            //添加信号，先将pid和ip加入限制，再运行子程序
            hashtable_add(hash_pid_ip,&pid,sizeof(pid),&addr.sin_addr.s_addr,sizeof(addr.sin_addr.s_addr));
            close(conn);
        }
    }
        

    hashtable_destroy(&hash_ip_clients);
    hashtable_destroy(&hash_pid_ip);
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

void check_clients(session_t* sess)
{
    if ( tunable_max_per_ip > 0 && sess->ip_clients > tunable_max_per_ip)
    {
        ftp_reply(sess,FTP_TOO_MANY_USERS,"There are too many connected users, please try later.");
        exit(EXIT_FAILURE);
    }
    if ( tunable_max_clients > 0 && sess->clients > tunable_max_clients )
    {
        ftp_reply(sess,FTP_TOO_MANY_USERS,"There are too many connected users, please try later.");
        exit(EXIT_FAILURE);
    }
}
/*
 * 接收子进程退出消息，维护clients变量和每ip客户端数目
 */
void handle_sigchld(int signal)
{
    pid_t pid;
    while ( ( pid=waitpid( -1,NULL,WNOHANG)) > 0 )
    {
       clients--; 
       del_client(pid);
    }
}
int get_clients_num(unsigned int ip)
{
    int* ret = hashtable_search(hash_ip_clients,&ip,sizeof(ip));
    if ( NULL == ret )
        return 0;
    else
        return *ret;    
}
int add_client(unsigned int ip)
{
    int* ret = hashtable_search(hash_ip_clients,&ip,sizeof(ip));
    if ( NULL != ret ) 
    {
        *ret = *ret + 1;
        return *ret;
    }
    else 
    {
        int val = 1;
        hashtable_add(hash_ip_clients,&ip,sizeof(ip),&val,sizeof(ret));
        return 1;
    }
}
int del_client(pid_t pid)
{
    unsigned int* ip = hashtable_search(hash_pid_ip,&pid,sizeof(pid));
    int* ret = hashtable_search(hash_ip_clients,ip,sizeof(*ip));
    if ( 1 == *ret )
        hashtable_del(hash_ip_clients,ip,sizeof(*ip));
    else 
        *ret = *ret - 1;

    hashtable_del(hash_pid_ip,&pid,sizeof(pid));

    return 0;
}
unsigned int hash_fun_limit(void* key,int hashtable_size)
{
    unsigned int* ip = key;
    return *ip % hashtable_size;
}

