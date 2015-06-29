#include "common.h"
#include "tunable.h"
#include "test.h"


int printconf()
{
    printf("listen address:%s\n",tunable_listen_address);
    printf("listen_port:%u\n",tunable_listen_port);
    printf("max_clients:%u\n",tunable_max_clients);
    printf("max_per_ip:%u\n",tunable_max_per_ip);
    printf("accept_timeout:%u\n",tunable_accept_timeout);
    printf("connect_timeout:%u\n",tunable_connect_timeout);
    printf("idle_session_timeout:%u\n",tunable_idle_session_timeout);
    printf("data_connection_timeout:%u\n",tunable_data_connection_timeout);
    printf("local_umask:%o\n",tunable_local_umask);
    printf("upload_max_rate:%u\n",tunable_upload_max_rate);
    printf("download_max_rate:%u\n",tunable_download_max_rate);
    printf("pasv_enable:%d\n",tunable_pasv_enable);
    printf("port_enable:%d\n",tunable_port_enable);
    return 0;
}
