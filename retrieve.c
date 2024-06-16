#include "retrieve.h"

int retrieving(connection_t connection,int t_flag,int messageNUM){
    char command[BUFFER_SIZE];
    sprintf(command,"%s FETCH %d BODY.PEEK[]\r\n",get_tag(),messageNUM);
    int n = send_command(connection,command,t_flag);
    n_check(n);

    char* full_email = get_full_email(connection,t_flag);
    fprintf(stdout,"%s\n",full_email);

    free(full_email);
    return 0;
}


