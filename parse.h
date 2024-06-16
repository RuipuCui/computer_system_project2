#ifndef _PARSE_H_
#define _PARSE_H_



#include "utils.h"


#define ATTRIBUTE 1024

int parse(connection_t connection,int t_flag,int messageNum);
void header_format(char* s);

#endif