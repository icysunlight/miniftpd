#include "ftpnobody.h"
#include "ftpipc.h"
#include "sysutil.h"
#include "tunable.h"
#include "common.h"



void do_getdatafd(session_t* ssinf);
void set_minprivilage(void);
int capset(cap_user_header_t hdrp,cap_user_data_t datap)
{
    return syscall(__NR_capset,hdrp,datap);
}

int handle_pri(session_t* ssinf)
{

    set_minprivilage();
    int cmd;
    while (1)
    {
        cmd = ftpipc_recv_cmd(ssinf->nobodyfd);
        if ( 0 == cmd)
            exit(EXIT_SUCCESS);
        if ( -1 == cmd )
            exit(EXIT_FAILURE);
        switch (cmd)
        {
        case FTPIPC_GET_DATA_FD:
            do_getdatafd(ssinf);
            break;
        }
    }
    return 0;
}

void do_getdatafd(session_t* ssinf)
{
    char ip[128] = {0};
    int port;
    if ( -1 == ftpipc_recv_data(ssinf->nobodyfd,ip,128))
        return;
    if ( -1 == ftpipc_recv_data(ssinf->nobodyfd,&port,sizeof(int)))
       return;
    //printf("nobody recv data:oip:%s\t,%u\n",ip,port);


    struct sockaddr_in addr;
    if ( -1 == ftpipc_recv_data(ssinf->nobodyfd,&addr,sizeof(addr)))
        return;
    //printf("ip:%s\tport:%d\n",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port)); 



    int fd = tcp_clnt(ip,port);
    if ( -1 == connect_timeout(fd,&addr,tunable_connect_timeout))
    {
        ERR_EXIT("connect_timeout error");
    }
    //printf("connect success\n");
     

    if ( -1 == fd )
        ftpipc_send_ret(ssinf->nobodyfd,FTPIPC_RET_BAD);
    
    ftpipc_send_ret(ssinf->nobodyfd,FTPIPC_RET_OK);
    ftpipc_send_fd(ssinf->nobodyfd,fd); 

    close(fd);
}

void set_minprivilage(void)
{
    struct passwd* pwd = getpwnam("nobody");
    if ( NULL == pwd )
        ERR_EXIT("getpwnam error");
    if ( -1 == setegid(pwd->pw_gid))
        ERR_EXIT("setegid error"); 
    if ( -1 == seteuid(pwd->pw_uid))
        ERR_EXIT("seteuid error"); 

    struct __user_cap_header_struct cap_header;
    cap_header.version = _LINUX_CAPABILITY_VERSION_1;
    cap_header.pid = 0;
    struct __user_cap_data_struct cap_data;
    cap_data.effective = cap_data.permitted = 1 << CAP_NET_BIND_SERVICE;
    cap_data.inheritable = 0;
    if ( -1 == capset(&cap_header,&cap_data))
        ERR_EXIT("capset error");
}
