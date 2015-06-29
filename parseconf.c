#include "common.h"
#include "parseconf.h"
#include "tunable.h"
#include "str.h"

//字符串型配置变量
static struct conf_str_t
{   
    const char* conf_str_key;
    const char** conf_str_val;
}
conf_str_array[] = 
{
    {"listen_address", &tunable_listen_address},
    {NULL,NULL}
};
//整数型配置变量
static struct conf_uint_t
{
    const char* conf_uint_key;
    unsigned int* conf_uint_val;
}
conf_uint_array[] = 
{
    {"listen_port",&tunable_listen_port},
    {"max_clients",&tunable_max_clients},
    {"max_per_ip",&tunable_max_per_ip},
    {"accept_timeout",&tunable_accept_timeout},
    {"connect_timeout",&tunable_connect_timeout},
    {"idle_session_timeout",&tunable_idle_session_timeout},
    {"data_connection_timeout",&tunable_data_connection_timeout},
    {"local_umask",&tunable_local_umask},
    {"upload_max_rate",&tunable_upload_max_rate},
    {"download_max_rate",&tunable_download_max_rate},
    {NULL,NULL} 
};
//bool型配置变量
static struct conf_bool_t
{
    const char* conf_bool_key;
    int* conf_bool_val;
}
conf_bool_array[] = 
{
    {"pasv_enable",&tunable_pasv_enable},
    {"port_enable",&tunable_port_enable},
    {NULL,NULL} 
};


int parseconf_load_file(const char* file)
{
    FILE* pfile = fopen(file,"r");
    if (NULL == pfile)
    {
        fprintf(stderr, "config file open error\n");
        return -1;
    }

    char line[1024];
    while (NULL != fgets(line,1024,pfile))
    {
        if (-1 == parseconf_load_setting(line)) 
        {
            fprintf(stderr, "config file parse error");
        }
    }
    fclose(pfile);
    return 0;
}

int parseconf_load_setting(char* line)
{
    str_trim_crlf(line);

    while (' ' == *line)
        line++;

    if ('#' == *line)
        return 0;

    char config_key[128] = {0};
    char config_val[128] = {0};

    if ( 2 != str_split(line,config_key,config_val,'='))
    {
        fprintf(stderr, "config synax error:%s\n",line);
        return -1;
    }

    struct conf_str_t* conf_str = conf_str_array;
    while (NULL != conf_str->conf_str_key) 
    {
        if (0 == strcmp(conf_str->conf_str_key,config_key))
        {
            if (NULL != *(conf_str->conf_str_val))
                free(conf_str->conf_str_val);

            *(conf_str->conf_str_val) = strdup(config_val);
            return 0;
        } 
        conf_str++;
    }

    //查找整数型配置
    struct conf_uint_t* conf_uint = conf_uint_array;
    while ( NULL != conf_uint->conf_uint_key)
    {
        if (0 == strcmp(conf_uint->conf_uint_key,config_key))
        {
            unsigned int nval = 0;
            if ( '0' == config_val[0])
                nval = str_octal_to_uint(config_val);
            else
                nval = atoi(config_val);

            *(conf_uint->conf_uint_val) = nval;
            return 0;
        }
        conf_uint++;
    }


    //查找bool型配置
    struct conf_bool_t* conf_bool = conf_bool_array;
    while ( NULL != conf_bool->conf_bool_key)
    {
        if (0 == strcmp(conf_bool->conf_bool_key,config_key))
        {
            if ( 0 == strcmp(config_val,"yes")
              || 0 == strcmp(config_val,"YES")   
              || 0 == strcmp(config_val,"1"))
                *(conf_bool->conf_bool_val) = 1;
            if ( 0 == strcmp(config_val,"no")
              || 0 == strcmp(config_val,"NO")   
              || 0 == strcmp(config_val,"0"))
                *(conf_bool->conf_bool_val) = 0;

            return 0;
        }
        conf_bool++;
    }
    return 0;
}
