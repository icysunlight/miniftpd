#include "common.h"
#include "str.h"


void str_trim_crlf(char* str)
{
    char* pstr = str + strlen(str) - 1;
    while (*pstr == '\n' || *pstr == '\r' ) {
        *pstr-- = '\0';
    }
}
int str_split(char* str,char *cmd,char* arg,char delim)
{
    char* pos = strchr(str,delim);
    if (NULL == pos)
    {
        strncpy(cmd,str,strlen(str));
        return 1;
    }
    else
    {
        *pos = '\0';
        strncpy(cmd,str,strlen(str));
        strncpy(arg,pos + 1,strlen(pos + 1));
        return 2;
    }
}
void str_upper(char* str)
{
    while (*str) {
        *str = toupper(*str);
        str++;
    }
}
unsigned int str_octal_to_uint(char* str)
{
    unsigned int ret = 0;
    while (*str) 
    {
        if (*str >= '0' && *str <= '7') 
        {
            ret = ret * 8 + *str - '0';
        }
        else
            return 0;
        str++;
    }
    return ret;
}
int is_allspace(const char* str)
{
    while (*str) {
        if (*str++ != ' ') {
            return 0;
        }
    }    
    return 1;
}

