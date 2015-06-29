#ifndef _STR_H_
#define _STR_H_

void str_trim_crlf(char* str);
int str_split(char* str,char *cmd,char* arg,char delim);
void str_upper(char* str);
unsigned int str_octal_to_uint(char* str);
int is_allspace(const char* str);


#endif
