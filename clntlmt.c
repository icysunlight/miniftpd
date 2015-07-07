#include "clntlmt.h"
#include "hash.h"
#include "tunable.h"

hashtable_t* hash_ip_clients;
hashtable_t* hash_pid_ip;
int clients;
static unsigned int hash_fun_limit(void* key,int hashtable_size)
{
    unsigned int* ip = key;
    return *ip % hashtable_size;
}
static int clients_ip_get(unsigned int ip)
{
    int* ret = hashtable_search(hash_ip_clients,&ip,sizeof(ip));
    assert(ret != NULL);
    return *ret;
}
static int client_add(unsigned int ip)
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
int clntlmt_init()
{
    clients = 0;
    hash_ip_clients = hashtable_init(2 * tunable_max_clients,hash_fun_limit);
    hash_pid_ip = hashtable_init(2 * tunable_max_clients,hash_fun_limit);
    return 0;
}
int clntlmt_destroy()
{
    clients = 0;
    hashtable_destroy(&hash_ip_clients);
    hashtable_destroy(&hash_pid_ip);
    return 0;
}
void clntlmt_add(pid_t pid,unsigned int ip)
{
    hashtable_add(hash_pid_ip,&pid,sizeof(pid),&ip,sizeof(ip));
    client_add(ip);
    clients++;
}
void clntlmt_del(pid_t pid)
{
    unsigned int* ip = hashtable_search(hash_pid_ip,&pid,sizeof(pid));
    int* ret = hashtable_search(hash_ip_clients,ip,sizeof(*ip));
    if ( 1 == *ret )
        hashtable_del(hash_ip_clients,ip,sizeof(*ip));
    else 
        *ret = *ret - 1;

    hashtable_del(hash_pid_ip,&pid,sizeof(pid));
    clients--;
}
int clntlmt_check(unsigned int ip)
{
    if ( tunable_max_per_ip > 0 && clients_ip_get(ip) > tunable_max_per_ip)
    {
        return -1;
    }
    if ( tunable_max_clients > 0 && clients  > tunable_max_clients )
    {
        return -2;
    }
    return 0;
}
