#include "ftpproto.h"
#include "common.h"
#include "str.h"
#include "sysutil.h"
#include "ftpcodes.h"
#include "tunable.h"
#include "ftpipc.h"

char username[128];
int b_dataconn;
session_t* pssinf;

int get_datafd(session_t *sess);
int has_port(session_t *sess);
int has_pasv(session_t *sess);
void clear_tansfer(session_t* sess);
int get_tcpsocket(session_t* sess,const char* ip,int nport);
int is_dir(const char* path);
void transfer_list(int datafd,const char* pathname);

//空闲断开信号注册
int cmd_disconn_start();
int data_disconn_start();
void signal_dataalarm(int sigal);
void signal_cmdalarm(int sigal);
//处理紧急情况
void handle_sigurg(int signal);
void is_urg(session_t* sess);


static void do_user(session_t *sess);
static void do_pass(session_t *sess);
static void do_cwd(session_t *sess);
static void do_cdup(session_t *sess);
static void do_quit(session_t *sess);
static void do_port(session_t *sess);
static void do_pasv(session_t *sess);
static void do_type(session_t *sess);
static void do_stru(session_t *sess);
static void do_mode(session_t *sess);
static void do_retr(session_t *sess);
static void do_stor(session_t *sess);
static void do_appe(session_t *sess);
static void do_list(session_t *sess);
static void do_nlst(session_t *sess);
static void do_rest(session_t *sess);
static void do_abor(session_t *sess);
static void do_pwd(session_t *sess);
static void do_mkd(session_t *sess);
static void do_rmd(session_t *sess);
static void do_dele(session_t *sess);
static void do_rnfr(session_t *sess);
static void do_rnto(session_t *sess);
static void do_site(session_t *sess);
static void do_site_help(session_t* sess,char* arg);
static void do_site_umask(session_t* sess,char* arg);
static void do_site_chmod(session_t* sess,char* arg);
static void do_syst(session_t *sess);
static void do_feat(session_t *sess);
static void do_size(session_t *sess);
static void do_stat(session_t *sess);
static void do_noop(session_t *sess);
static void do_help(session_t *sess);

static struct ftp_cmd_t
{
    const char* fc_name;
    void (*fc_func)(session_t*);
}
ftp_cmd_arr[] = 
{
    {"USER",    do_user },
    {"PASS",    do_pass },
    {"CWD",     do_cwd },
    {"XCWD",    do_cwd },
    {"CDUP", do_cdup },
    {"XCUP", do_cdup },
    {"QUIT",    do_quit },
    {"ACCT", NULL },
    {"SMNT", NULL },
    {"REIN",NULL },
    /* 传输参数命令 */
    {"PORT", do_port },
    {"PASV", do_pasv },
    {"TYPE", do_type },
    {"STRU", do_stru },
    {"MODE", do_mode },
    /* 服务命令 */
    {"RETR", do_retr },
    {"STOR", do_stor },
    {"APPE",do_appe },
    {"LIST", do_list },
    {"NLST", do_nlst },
    {"REST",do_rest },
    {"ABOR", do_abor },
    {"\377\364\377\362ABOR", do_abor},
    {"PWD",     do_pwd },
    {"XPWD",    do_pwd },
    {"MKD",     do_mkd },
    {"XMKD", do_mkd },
    {"RMD", do_rmd },
    {"XRMD", do_rmd },
    {"DELE", do_dele },
    {"RNFR",    do_rnfr },
    {"RNTO",    do_rnto },
    {"SITE", do_site },
    {"SYST",    do_syst },
    {"FEAT",    do_feat },
    {"SIZE", do_size },
    {"STAT", do_stat },
    {"NOOP", do_noop },
    {"HELP", do_help },
    {"STOU", NULL },
    {"ALLO", NULL }
};

int ftp_reply(session_t* ssinf,int status,const char* text)
{
    char buf[1024] = {0};
    sprintf(buf,"%d %s\r\n",status,text);
    if ( -1 == writen(ssinf->ctrlfd,buf,strlen(buf)))
        ERR_EXIT("written error");
    return 0;
}

int ftp_lreply(session_t* ssinf,int status,const char* text)
{
    char buf[1024] = {0};
    sprintf(buf,"%d-%s\r\n",status,text);
    if ( -1 == writen(ssinf->ctrlfd,buf,strlen(buf)))
        ERR_EXIT("written error");
    return 0;
}

int get_tcpsocket(session_t* sess,const char* ip,int nport)
{
    ftpipc_send_cmd(sess->protofd,FTPIPC_GET_DATA_FD);
    
    ftpipc_send_data(sess->protofd,ip,strlen(ip));
    ftpipc_send_data(sess->protofd,&nport,sizeof(int));

    ftpipc_send_data(sess->protofd,sess->port_addr,sizeof(struct sockaddr_in));

    int ret = ftpipc_recv_ret(sess->protofd);
    if ( FTPIPC_RET_OK == ret )
    {
        return ftpipc_recv_fd(sess->protofd);
    }
    else
        return -1;
}

void getfileper(struct stat* fbuf,char* per)
{
    switch ( fbuf->st_mode & S_IFMT )
    {
    case S_IFSOCK:
        per[0] = 's';
        break;     
    case S_IFLNK:
        per[0] = 'l';
        break;     
    case S_IFREG:
        per[0] = '-';
        break;     
    case S_IFBLK:
        per[0] = 'b';
        break;     
    case S_IFDIR:
        per[0] = 'd';
        break;     
    case S_IFCHR:
        per[0] = 'c';
        break;     
    case S_IFIFO:
        per[0] = 'p';
        break;     
    }
    
    if ( fbuf->st_mode & S_IRUSR )
        per[1] = 'r';
    if ( fbuf->st_mode & S_IRUSR )
        per[2] = 'w';
    if ( fbuf->st_mode & S_IXUSR )
        per[3] = 'x';
    if ( fbuf->st_mode & S_IRGRP )
        per[4] = 'r';
    if ( fbuf->st_mode & S_IRGRP )
        per[5] = 'w';
    if ( fbuf->st_mode & S_IXGRP )
        per[6] = 'x';
    if ( fbuf->st_mode & S_IROTH )
        per[7] = 'r';
    if ( fbuf->st_mode & S_IROTH )
        per[8] = 'w';
    if ( fbuf->st_mode & S_IXOTH )
        per[9] = 'x';

    if ( fbuf->st_mode & S_ISUID)
        per[3] = (per[3] == 'x') ? 's' : 'S';
    if ( fbuf->st_mode & S_ISGID)
        per[6] = (per[6] == 'x') ? 's' : 'S';
    if ( fbuf->st_mode & S_ISVTX)
        per[9] = (per[9] == 'x') ? 't' : 'T';
}
void transfer_list(int datafd,const char* pathname)
{

    DIR* dir = opendir(pathname);
    if (NULL == dir)
       ERR_EXIT("opendir error");

    struct dirent* entry;
    while ( NULL != (entry = readdir(dir)))
    {
        if ('.' == entry->d_name[0]) 
            continue;

        struct stat fbuf;
        if (-1 == lstat(entry->d_name,&fbuf))
                ERR_EXIT("lstat error");

        char fstatbuf[1024] = {0};
        int off = 0;
        char per[11] = "----------";
        getfileper(&fbuf,per);
        per[10] = '\0';
        off += sprintf(fstatbuf + off,"%s",per);
        off += sprintf(fstatbuf + off," %d",(int)fbuf.st_nlink);
        off += sprintf(fstatbuf + off," %d",fbuf.st_gid);
        off += sprintf(fstatbuf + off," %d",fbuf.st_uid);
        off += sprintf(fstatbuf + off," %d",(int)fbuf.st_size);

        struct tm* tm = gmtime(&fbuf.st_atime);
        char strtime[128];
        strftime(strtime,128,"%b %d %H:%M",tm);
        off += sprintf(fstatbuf + off," %s",strtime);
        off += sprintf(fstatbuf + off," %s",entry->d_name);

        if ( 'l' == per[0] )
        {
            char lcontent[128] = {0};
            readlink(entry->d_name,lcontent,128);
            off += sprintf(fstatbuf + off," -> %s",lcontent);
        }

        off += sprintf(fstatbuf + off,"\r\n");
        writen(datafd,fstatbuf,off);
        fstatbuf[off + 1] = '\0';
        //printf("%s\n",fstatbuf);
    }

    closedir(dir);
}
int is_dir(const char* path)
{
    struct stat filebuf;
    if ( -1 == stat(path,&filebuf) )
        return -1;
    return S_ISDIR(filebuf.st_mode);
}
int cmd_disconn_start()
{
    signal(SIGALRM,signal_cmdalarm);
    if ( 0 != tunable_idle_session_timeout) 
    {
        alarm(tunable_idle_session_timeout);
        //printf("alarm time:%u\n",tunable_idle_session_timeout);
    }
    return 0;
}
int data_disconn_start()
{
    b_dataconn = 0;
    signal(SIGALRM,signal_dataalarm);
    if ( 0 != tunable_data_connection_timeout) 
    {
        alarm(tunable_data_connection_timeout);
    }
    return 0;
}
void signal_cmdalarm(int sigal)
{
    shutdown(pssinf->ctrlfd,SHUT_RD);
    ftp_reply(pssinf,FTP_IDLE_TIMEOUT,"Time out");
    shutdown(pssinf->ctrlfd,SHUT_WR);
    exit(EXIT_SUCCESS);
}
void signal_dataalarm(int sigal)
{
    if ( 0 == b_dataconn ) 
    {
        clear_transfer(pssinf);
    }
    else 
    {
        b_dataconn = 0;
        alarm(tunable_data_connection_timeout);
    }
}
void handle_sigurg(int signal)
{        
    memset(pssinf->cmdline,0,sizeof(char) * MAXCMDLINE);
    memset(pssinf->cmd,0,sizeof(char) * MAXCMDLINE);
    memset(pssinf->arg,0,sizeof(char) * MAXCMDLINE);
    
    int ret = readline(pssinf->ctrlfd,pssinf->cmdline,MAXCMDLINE);
    if (-1 == ret )
        ERR_EXIT("read error");
    if (0 == ret)
        exit(EXIT_SUCCESS);

    str_trim_crlf(pssinf->cmdline);
    str_split(pssinf->cmdline,pssinf->cmd,pssinf->arg,' ');
    str_upper(pssinf->cmd);
    printf("[%s]\t[%s]\n",pssinf->cmd,pssinf->arg);

    if ( 0 == strcmp("ABOR",pssinf->cmd) || 0 == strcmp(pssinf->cmd,"\377\364\377\362ABOR"))
    {
        clear_transfer(pssinf);
        pssinf->burg = 1;        
    }
    else 
    {
        ftp_reply(pssinf,FTP_BADCMD,"Unkown command");
    }
}

void is_urg(session_t* ssinf)
{
    if ( 1 == ssinf-> burg ) 
    {
        ssinf->burg = 0;
        ftp_reply(ssinf,FTP_ABOROK,"Abor OK.");
    }
}

int handle_ftp(session_t* ssinf)
{
    ftp_reply(ssinf,220,"miniftp v0.1");
    pssinf = ssinf;
    b_dataconn = 0;

    //开启紧急数据接收
    if (activate_oobinline(ssinf->ctrlfd) == -1 )
        ERR_EXIT("active_oobinline error.");
    if (activate_sigurg(ssinf->ctrlfd) == -1 )
        ERR_EXIT("activate sigurg error.");
    if (signal(SIGURG,handle_sigurg) == SIG_ERR )
        ERR_EXIT("signal error.");

    while (1)
    {
        memset(ssinf->cmdline,0,sizeof(char) * MAXCMDLINE);
        memset(ssinf->cmd,0,sizeof(char) * MAXCMDLINE);
        memset(ssinf->arg,0,sizeof(char) * MAXCMDLINE);

        cmd_disconn_start();

        int ret = readline(ssinf->ctrlfd,ssinf->cmdline,MAXCMDLINE);
        if ( -1 == ret && errno == EINTR )
            continue;
        if (-1 == ret )
            ERR_EXIT("read error");
        if (0 == ret)
            exit(EXIT_SUCCESS);
 
        str_trim_crlf(ssinf->cmdline);
        str_split(ssinf->cmdline,ssinf->cmd,ssinf->arg,' ');
        str_upper(ssinf->cmd);
        printf("[%s]\t[%s]\n",ssinf->cmd,ssinf->arg);

        int arrsize = sizeof(ftp_cmd_arr) / sizeof(struct ftp_cmd_t);
        int i = 0;
        while (i < arrsize)
        {
            if ( 0 == strcmp(ftp_cmd_arr[i].fc_name,ssinf->cmd))
            {
                if (NULL == ftp_cmd_arr[i].fc_func)
                {
                    ftp_reply(ssinf,FTP_COMMANDNOTIMPL,"command not implemented");
                }
                else 
                {
                    ftp_cmd_arr[i].fc_func(ssinf);
                }
                break;
            }
            i++;
        }
        
        if (i == arrsize)
        {
            ftp_reply(ssinf,FTP_BADCMD,"Unknown Command."); 
        }
    }
}

int get_datafd(session_t *sess)
{

    if ( has_port(sess))
    {
        int fd = get_tcpsocket(sess,listenip,20);

        //fd = tcp_clnt("192.168.1.3",0);
        /*
        struct sockaddr_in addr;
        socklen_t socklen;
        if ( -1 == getsockname(fd,(struct sockaddr*)&addr,&socklen))
            ERR_EXIT("getsockname error");
        printf("fd:%d\tip:%s\tport:%d\n",fd,inet_ntoa(addr.sin_addr),ntohs(addr.sin_port)); 
        if ( -1 == connect_timeout(fd,sess->port_addr,tunable_connect_timeout))
        {
            ERR_EXIT("connect_timeout error");
        }
        printf("connect success\n");
        */
        return fd;
    }
    
    if ( has_pasv(sess))
    {
        int fd = accept_timeout(sess->pasv_lsfd,NULL,tunable_accept_timeout);
        return fd; 
    }
    return -1;
}

static void do_user(session_t* ssinf)
{
    memset(username,0,sizeof(char) * 128); 
    strcpy(username,ssinf->arg);
    ftp_reply(ssinf,331,"Please specify the passwd");
}
static void do_pass(session_t* ssinf)
{
    struct spwd* spw = getspnam(username);
    struct passwd* pw = getpwnam(username);
    if (NULL == spw || NULL == pw) 
    {
        ftp_reply(ssinf,530,"Login incorrect");
    }

    char* encriedpw = crypt(ssinf->arg,spw->sp_pwdp);
    if ( 0 == strcmp(encriedpw,spw->sp_pwdp))
    {
        printf("login success\tusername:%s\n",username);    
        ftp_reply(ssinf,230,"Login successful");
        setegid(pw->pw_gid);
        seteuid(pw->pw_uid);
        chdir(pw->pw_dir);
    }
    else
    {
        ftp_reply(ssinf,530,"Login incorrect");
    }
}
void do_cwd(session_t *sess)
{
    if ( -1 == chdir(sess->arg))
        ftp_reply(sess,FTP_CWDFAIL,"Failed to change directory.");
    else
        ftp_reply(sess,FTP_CWDOK,"Directory successfully changed.");
    
}
void do_cdup(session_t *sess)
{
    if ( -1 == chdir(".."))
        ftp_reply(sess,FTP_CWDFAIL,"Failed to change directory.");
    else
        ftp_reply(sess,FTP_CWDOK,"Directory successfully changed.");
}
void do_quit(session_t *sess)
{
    ftp_reply(sess,FTP_GOODBYE,"Good Bye.");
    exit(EXIT_SUCCESS);
}
void do_port(session_t *sess)
{
    clear_transfer(sess);

    int naddr[6];  
    sscanf(sess->arg,"%d,%d,%d,%d,%d,%d",&naddr[0],&naddr[1],&naddr[2],&naddr[3],&naddr[4],&naddr[5]);
    char ip[12] = {0};
    sprintf(ip,"%d.%d.%d.%d",naddr[0],naddr[1],naddr[2],naddr[3]);

    unsigned short nport = naddr[4];
    nport = (nport << 8) + naddr[5];


    //printf("addr:\tip:%s\tport:%d\n",ip,nport); 
    sess->port_addr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));

    sess->port_addr->sin_family = AF_INET;
    sess->port_addr->sin_port = htons(nport);
    sess->port_addr->sin_addr.s_addr = (inet_addr(ip));;


    ftp_reply(sess,FTP_PORTOK,"Port commande successful.");
}
void do_pasv(session_t *sess)
{
    clear_transfer(sess);

    sess->pasv_lsfd = tcp_srv(listenip,0);
    
    struct sockaddr_in srvaddr;
    socklen_t socklen = sizeof(srvaddr);
    if ( -1 == getsockname(sess->pasv_lsfd,(struct sockaddr*)&srvaddr,&socklen))
        ERR_EXIT("getsockname error");
    

    char buf[1024];
    unsigned int port = ntohs(srvaddr.sin_port);
    unsigned int naddr[4];
    sscanf(inet_ntoa(srvaddr.sin_addr),"%u.%u.%u.%d",&naddr[0],&naddr[1],&naddr[2],
            &naddr[3]);

    sprintf(buf,"Entering Passive Mode (%u,%u,%u,%u,%u,%u).",naddr[0],naddr[1],naddr[2],naddr[3],
                port >> 8 , port & 0xFF);

    //printf("port :%u\n",port);
    
    ftp_reply(sess,FTP_PASVOK,buf);


}
void do_type(session_t *sess)
{
    //printf("%s\n",sess->arg);
    if ( 0 == strcmp("A",sess->arg))
    {
        //sess->bASCII = 1;
        //ftp_reply(sess,FTP_TYPEOK,"Switching to ASCII mode.");
        ftp_reply(sess,504,"ASCII mode unimplement");
    }
    else if ( 0 == strcmp("I",sess->arg))
    {
        sess->bASCII = 0;
        ftp_reply(sess,FTP_TYPEOK,"Switching to Binary mode.");
    }
    else
    {
        ftp_reply(sess,FTP_BADCMD,"Unrecognised TYPE command.");
    }
}
void do_stru(session_t *sess)
{

}
void do_mode(session_t *sess)
{

}

void do_retr(session_t *sess)
{
    //打开数据连接
    int data_fd = get_datafd(sess);
    sess->datafd = data_fd;
    if ( -1 == data_fd )
    {
        ftp_reply(sess,FTP_BADSENDCONN,"Use port or pasv first");
        return;
    }

    //打开文件并加锁
    if ( is_dir(sess->arg) )
    {
        ftp_reply(sess,FTP_BADSENDFILE,"Read local file fail.");
        return;
    }
    int filefd = open(sess->arg,O_RDONLY);
    if ( -1 == filefd )
    {
        ftp_reply(sess,FTP_BADSENDFILE,"Read local file fail.");
        return;
    }
    if ( -1 == file_rdlock(filefd)) 
    {
        ftp_reply(sess,FTP_BADSENDFILE,"Read local file fail.");
        close(filefd);
        return;
    }

    //读取文件大小
    off_t filesize = lseek(filefd,0,SEEK_END);
    lseek(filefd,sess->rest_pos,SEEK_SET);
    
    //发送开始信息
    char buf[2048] = {0};
    sprintf(buf,"Openning BINARY mode data connect for %s (%lld bytes)",sess->arg,(long long)filesize);
    ftp_reply(sess,FTP_DATACONN,buf);

    int flag = 0;
    //发送文件
    if ( 0 == sess->bASCII)
    {
        off_t sizetosend = filesize - sess->rest_pos;
        while ( 0 != sizetosend )
        {
            ratelim_init(sess);
            data_disconn_start();
            ssize_t sizecur = sizetosend > 2048 ? 2048 : sizetosend;
            ssize_t ret = sendfile(data_fd,filefd,NULL,sizecur);
            if ( -1 == ret )
            {
                flag = 1;
                break; 
            } 
            sizetosend -= ret;
            ratelim_do(sess,tunable_download_max_rate,ret);
            b_dataconn = 1; 
        }
    }

    switch (flag)
    {
        case 0:
            ftp_reply(sess,FTP_TRANSFEROK,"Transfer complete.");
            break;
        case 1:
            ftp_reply(sess,FTP_BADSENDNET,"Send file failure.");
            break;
    }

    is_urg(sess);
    file_unlock(filefd);
    close(data_fd);
    close(filefd);
    clear_transfer(sess);
    sess->rest_pos = 0;
}
void do_stor(session_t *sess)
{
    //打开数据连接
    sess->datafd = get_datafd(sess);
    if ( -1 == sess->datafd )
    {
        ftp_reply(sess,FTP_BADSENDCONN,"Use port or pasv first");
        return;
    }

    //打开文件并加锁
    int filefd = open(sess->arg,O_WRONLY | O_CREAT , 0666);
    if ( -1 == filefd )
    {
        ftp_reply(sess,FTP_BADSENDFILE,"Create local file fail.");
        ERR_EXIT("open");
        return;
    }
    if ( -1 == file_wrlock(filefd)) 
    {
        ftp_reply(sess,FTP_BADSENDFILE,"Create local file fail.");
        close(filefd);
        return;
    }

    //定位ref
    if ( sess->rest_pos != 0)
        lseek(filefd,sess->rest_pos,SEEK_SET);
    else if ( sess->bappe ) 
    {
        lseek(filefd,0,SEEK_END);
    }
    else 
    {
        ftruncate(filefd,0);
    }
    
    //发送开始信息
    ftp_reply(sess,FTP_DATACONN,"OK to send data");

    int flag = 0;
    //发送文件
    if ( 0 == sess->bASCII)
    {
        while (1)
        {
            ratelim_init(sess);
            data_disconn_start();
            char buf[1024] = {0};
            int ret = read(sess->datafd,buf,1024);
            if ( -1 == ret && EINTR == errno )
                continue;
            if ( -1 == ret )
            {
                flag = 1;
                break;
            }
            if ( 0 == ret )
            {
                flag = 0;
                break;
            }

            b_dataconn = 1;
            ratelim_do(sess,tunable_upload_max_rate,ret); 
    
            if ( ret != writen(filefd,buf,ret) )
            {
                flag = 2;
                break;
            }
        }
    }

    switch (flag)
    {
        case 0:
            ftp_reply(sess,FTP_TRANSFEROK,"Transfer complete.");
            break;
        case 1:
            ftp_reply(sess,FTP_BADSENDNET,"Receive file failure.");
            break;
        case 2:
            ftp_reply(sess,FTP_BADSENDFILE,"Write local file failure.");
            break;
    }

    is_urg(sess);
    file_unlock(filefd);
    close(filefd);
    clear_transfer(sess);
}
void do_appe(session_t *sess)
{
   sess->bappe = 1;
}
void do_list(session_t *sess)
{
    int data_fd = get_datafd(sess);
    if ( -1 == data_fd )
    {
        ftp_reply(sess,FTP_BADSENDCONN,"Use port or pasv first");
        return;
    }

    char buf[1024];
    getcwd(buf,1024); 
    ftp_reply(sess,FTP_DATACONN,"Here comes the dictionary listing.");
    transfer_list(data_fd,buf);
    close(data_fd);
    ftp_reply(sess,FTP_TRANSFEROK,"Directory send OK.");
    clear_transfer(sess);
}
void do_nlst(session_t *sess)
{

}
void do_rest(session_t *sess)
{
    sscanf(sess->arg,"%lld",&sess->rest_pos);
    char replybuf[128];
    sprintf(replybuf,"Restart position accepted (%lld).",sess->rest_pos);
    ftp_reply(sess,FTP_RESTOK,replybuf);
}
void do_abor(session_t *sess)
{
    ftp_reply(sess,FTP_ABOR_NOCONN,"No transfer to abor");
}
void do_pwd(session_t *sess)
{
    char buf[1024];
    getcwd(buf,1024); 
    char tmp[1024];
    sprintf(tmp, "\"%s\"",buf);
    ftp_reply(sess,FTP_PWDOK,tmp);
}
void do_mkd(session_t *sess)
{
    if ( -1 == mkdir(sess->arg,0777) )
    {
        ftp_reply(sess,FTP_FILEFAIL,"Create dictionary operation filed");
    }
    else 
    {
        char buf[1024] = {0};
        sprintf(buf,"\"%s\" created.",sess->arg);
        ftp_reply(sess,FTP_MKDIROK,buf);
    }
}
void do_rmd(session_t *sess)
{
    if ( -1 == rmdir(sess->arg) )
    {
        ftp_reply(sess,FTP_FILEFAIL,"Remove dictionary operation filed");
    }
    else 
    {
        ftp_reply(sess,FTP_MKDIROK,"Remove dictionary operation success");
    }
}
void do_dele(session_t *sess)
{
    if ( 0 == unlink(sess->arg) )
    {
        ftp_reply(sess,FTP_DELEOK,"Delete operation successful.");
    }
    else 
    {
        ftp_reply(sess,FTP_DELEOK,"Delete operation failed.");
    }
}
void do_rnfr(session_t *sess)
{
    if ( NULL == sess->rn_filepath ) 
        sess->rn_filepath = (char*)malloc(strlen(sess->arg));
    else 
    {
        free(sess->rn_filepath);
        sess->rn_filepath = (char*)malloc(strlen(sess->arg) + 2);
    }

    strcpy(sess->rn_filepath,sess->arg);

    ftp_reply(sess,FTP_RESTOK,"Ready for RNTO.");
}
void do_rnto(session_t *sess)
{
    if ( NULL == sess->rn_filepath) 
    {
        ftp_reply(sess,FTP_NEEDRNFR,"RNFR required first.");
        return;
    }

    if ( -1 == rename(sess->rn_filepath,sess->arg))
    {
        printf("oripath:%s\t,detpath:%s\n",sess->rn_filepath,sess->arg);
        ftp_reply(sess,FTP_FILEFAIL,"Rename failed.");
    }
    else
        ftp_reply(sess,FTP_RENAMEOK,"Rename successful.");

    free(sess->rn_filepath);
    sess->rn_filepath = NULL;
}
void do_site(session_t *sess)
{
    char cmd[128] = {0};
    char arg[128] = {0};
    if ( 0 == strcmp("",sess->arg ))
    {
        ftp_reply(sess,FTP_BADCMD,"Unkown SITE commond.");
        return;
    }
    str_split(sess->arg,cmd,arg,' ');
    str_upper(cmd);

    if ( 0 == strcmp(cmd,"HELP") )
        do_site_help(sess,arg);
    else if ( 0 == strcmp(cmd,"UMASK") )
        do_site_umask(sess,arg);
    else if ( 0 == strcmp(cmd,"CHMOD") )
        do_site_chmod(sess,arg);
    else
        ftp_reply(sess,FTP_BADCMD,"Unkown SITE commond.");
}
void do_site_help(session_t* sess,char* cmdline)
{
    ftp_reply(sess,FTP_SITEHELP,"CHMOD UMASK HELP");
}
void do_site_umask(session_t* sess,char* cmdline)
{
    char str[128] = {0};
    if ( 0 == strlen(cmdline) )
    {
        sprintf(str,"Your current UMASK is 0%o",tunable_local_umask);
        ftp_reply(sess,FTP_UMASKOK,str);
    }
    else
    {
        unsigned int maskval = str_octal_to_uint(cmdline);
        printf("%u\n",maskval);
        umask(maskval); 
        sprintf(str,"UMASK set to %o",maskval);
        ftp_reply(sess,FTP_UMASKOK,str);
    }
}
void do_site_chmod(session_t* sess,char* arg)
{
    char perm[128] = {0};
    char file[128] = {0};
    
    if ( 2 != str_split(arg,perm,file,' ') )
    {
        ftp_reply(sess,FTP_BADCMD,"SITE CHMOD needs 2 arguments.");
        return;
    }

    unsigned int mode = str_octal_to_uint(perm);
    if ( -1 == chmod(file,mode) )
        ftp_reply(sess,FTP_CHMODFAIL,"SITE CHMODE command failed");
    else
        ftp_reply(sess,FTP_CHMODOK,"SITE CHMODE command ok");

}
void do_syst(session_t *sess)
{
    ftp_reply(sess,FTP_SYSTOK,"Unit Type: L8");
}
void do_feat(session_t *sess)
{
    ftp_lreply(sess,FTP_FEAT,"Features");
    writen(sess->ctrlfd," EPRT\r\n",strlen(" EPRT\r\n"));
    writen(sess->ctrlfd," EPSV\r\n",strlen(" EPSV\r\n"));
    writen(sess->ctrlfd," MDTM\r\n",strlen(" MDTM\r\n"));
    writen(sess->ctrlfd," PASV\r\n",strlen(" PASV\r\n"));
    writen(sess->ctrlfd," REST STREAM\r\n",strlen(" REST STREAM\r\n"));
    writen(sess->ctrlfd," SIZE\r\n",strlen(" SIZE\r\n"));
    writen(sess->ctrlfd," TVFS\r\n",strlen(" PASV\r\n"));
    writen(sess->ctrlfd," UTF8\r\n",strlen(" UTF8\r\n"));
    ftp_reply(sess,FTP_FEAT,"End");
}
void do_size(session_t *sess)
{
    struct stat statbuf;
    if ( -1 == stat(sess->arg,&statbuf) )
    {
        ftp_reply(sess,FTP_FILEFAIL,"Could not get file size.");
        return;
    }

    if ( !S_ISREG(statbuf.st_mode) )
    {
        ftp_reply(sess,FTP_FILEFAIL,"Could not get file size.");
        return;
    }

    char sizebuf[128] = {0};
    sprintf(sizebuf,"%d",(int)statbuf.st_size);
    ftp_reply(sess,FTP_SIZEOK,sizebuf);
}
void do_stat(session_t *sess)
{
    ftp_lreply(sess,FTP_STATOK,"FTP server status:");
    char buf[1024] = {0};

    struct in_addr inaddr;
    inaddr.s_addr = (sess->ip);
    sprintf(buf," Connected to %s\r\n",inet_ntoa(inaddr));
    writen(sess->ctrlfd,buf,strlen(buf));

    memset(buf,0,sizeof(char) * 1024);
    sprintf(buf," Logged in as %s\r\n",username);
    writen(sess->ctrlfd,buf,strlen(buf));
    
    memset(buf,0,sizeof(char) * 1024);
    if ( sess->bASCII )
        sprintf(buf," TYPE: ASCII \r\n");
    else
        sprintf(buf," TYPE: BINARY \r\n");
    writen(sess->ctrlfd,buf,strlen(buf));

    memset(buf,0,sizeof(char) * 1024);
    if ( 0 == tunable_connect_timeout ) 
        sprintf(buf," No session timeout\r\n");
    else
        sprintf(buf," Session timeout in seconds is %u\r\n",tunable_connect_timeout);
    writen(sess->ctrlfd,buf,strlen(buf));

    ftp_reply(sess,FTP_STATOK,"End of status");
}
void do_noop(session_t *sess)
{
    ftp_reply(sess,FTP_OPTSOK,"Noop OK");
}
void do_help(session_t *sess)
{
    ftp_lreply(sess,FTP_HELP,"The following commands are recognized.");
    writen(sess->ctrlfd," ABOR ACCT ALLO APPE CDUP CWD  DELE EPRT EPSV FEAT HELP LIST MDTM MKD\r\n",
            strlen(" ABOR ACCT ALLO APPE CDUP CWD  DELE EPRT EPSV FEAT HELP LIST MDTM MKD\r\n"));
    writen(sess->ctrlfd," MODE NLST NOOP OPTS PASS PASV PORT PWD  QUIT REIN REST RETR RMD  RNFR\r\n",
            strlen(" MODE NLST NOOP OPTS PASS PASV PORT PWD  QUIT REIN REST RETR RMD  RNFR\r\n"));
    writen(sess->ctrlfd," RNTO SITE SIZE SMNT STAT STOR STOU STRU SYST TYPE USER XCUP XCWD XMKD\r\n",
            strlen(" RNTO SITE SIZE SMNT STAT STOR STOU STRU SYST TYPE USER XCUP XCWD XMKD\r\n"));
    writen(sess->ctrlfd," XPWD XRMD\r\n",strlen(" XPWD XRMD\r\n"));
    ftp_reply(sess,FTP_HELP,"HELP OK");
}
