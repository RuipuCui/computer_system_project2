#ifndef _UTILS_H
#define _UTILS_H

#include "socket.h"
#include <ctype.h>

#define HEADER_BUFFER 255

int get_message_length( char* header);
char* get_full_email(connection_t connection,int t_flag);
char* get_full_response(connection_t connection,int t_flag);
void replace_crlf_with_space(char* original);
void checkInvalidArgument(char* argu);
void check_memory(char* s);
void n_check(int n);
char *my_strstr(const char *haystack, const char *needle);
void check_isdigit(char* s);
#endif