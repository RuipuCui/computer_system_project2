#ifndef _MIME_H_
#define _MIME_H_

#include "socket.h"

int mime(connection_t connection, int t_flag,int messageNUM);
int get_part(char* full_email);
#endif