#include "session.h"
#include "sysutil.h"
#include "str.h"
#include "ftpproto.h"
#include "ftpnobody.h"
#include "common.h"
#include "ftpipc.h"


void init_session(session_t* ssinf,int conn)
{
    memset(ssinf,0,sizeof(session_t));
    ssinf->ctrlfd = conn;
    ssinf->protofd = ssinf->nobodyfd = -1;

    ssinf->bASCII = 0;

    ssinf->rn_filepath = NULL;
    ssinf->pasv_lsfd = -1;

    ssinf->burg = 0;

    ssinf->clients = 0;
    ssinf->ip_clients = 0;

    ssinf->ip = 0;
}

int begin_session(session_t* sessioninf)
{

    ftpipc_init(sessioninf);

    pid_t pid = fork();
    if (-1 == pid)
        ERR_EXIT("fork error");

    if (pid > 0)
    {
        //nobody process
        //set the process nobody
        ftpipc_setnobody(sessioninf);
        handle_pri(sessioninf);
    }
    if (0 == pid)
    {
        //ftp proto
        ftpipc_setproto(sessioninf);
        handle_ftp(sessioninf);
    }
    
    return 0;
}

void clear_session(session_t* ssinf)
{
    clear_transfer(ssinf);

    if ( -1 != ssinf->nobodyfd )
        close(ssinf->nobodyfd); 
    if ( -1 != ssinf->protofd )
        close(ssinf->protofd); 
    if ( -1 != ssinf->ctrlfd )
        close(ssinf->ctrlfd); 
    if ( -1 != ssinf->datafd )
        close(ssinf->datafd); 
}

void clear_transfer(session_t* ssinf)
{
    shutdown(ssinf->datafd,SHUT_RDWR);
    ssinf->datafd = -1;
    if ( has_port(ssinf) )
    {
        free(ssinf->port_addr);
        ssinf->port_addr = NULL;
    }
    if ( has_pasv(ssinf) )
    {
        close(ssinf->pasv_lsfd);
        ssinf->pasv_lsfd = -1;
    }
    if ( NULL != ssinf->rn_filepath )
        free(ssinf->rn_filepath);
    ssinf->rest_pos = 0;
    ssinf->bappe = 0;
}
int has_port(session_t *sess)
{
    return sess->port_addr != NULL;
}
int has_pasv(session_t *sess)
{
    return sess->pasv_lsfd != -1;
}
int ratelim_init(session_t* sess)
{
    struct timeval tv;
    if ( -1 == gettimeofday(&tv,NULL) ) 
        return -1;

    sess->ts_sec = tv.tv_sec;
    sess->ts_usec = tv.tv_usec;
    return 0;
}
int ratelim_do(session_t* sess,unsigned int maxrate,int nbytes)
{
    if ( 0 == maxrate )
        return 0;
    struct timeval tv;
    if ( -1 == gettimeofday(&tv,NULL) ) 
        return -1;

    double elapsed = (double)(tv.tv_sec - sess->ts_sec) + 0.000001 * (double)(tv.tv_usec - sess->ts_usec);
    if ( elapsed <= 0 )
        elapsed = 0.01;

    double currate = (double)nbytes / elapsed;
    if ( (int)currate < maxrate )
        return 0;
    double timetosleep = ( currate / (double)maxrate - 1 ) * elapsed;


    struct timespec ts;
    ts.tv_sec = (int)timetosleep;
    ts.tv_nsec = (timetosleep - ts.tv_sec) * 1000000000;

    int ret;
    do 
    {
        ret = nanosleep(&ts,&ts);
    } while ( -1 == ret && EINTR == errno); 

    return 0;
}
