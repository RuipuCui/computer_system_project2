#ifndef _SOCKET_H_
#define _SOCKET_H_

#define LOGIN "LOGIN"
#define LIST "list"
#define RETRIEVE "retrieve"
#define PARSE "parse"
#define MIME "mime"
#define PORT "143"
#define TSL_PORT "993"
#define BUFFER_SIZE 255

#include "libs.h"

typedef struct 
{
    int connfd;
    SSL* ssl;
} connection_t;



#include "retrieve.h"
#include "parse.h"
#include "listing.h"
#include "mime.h"


void initialize_ssl();
SSL_CTX* create_ssl_context() ;
int set_connection(char* server,char* username, char* password, char* folder,char* task,int messageNum,int t_flag);
const char* get_tag();
int send_command(connection_t connection, const char *command, int flag);
int get_reply(connection_t connection, int flag, char *data, int data_size);
void clear_server_buffer(connection_t connection,int t_flag);


#endif