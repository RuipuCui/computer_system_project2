#include "mime.h"



int mime(connection_t connection, int t_flag,int messageNUM){
    //same as retrieve to get raw message
    char command[BUFFER_SIZE];
    //use body structure to find the part I need
    sprintf(command,"%s FETCH %d (BODYSTRUCTURE)\r\n",get_tag(),messageNUM);

    int n = send_command(connection,command,t_flag);
    n_check(n);

    char* full_email = get_full_response(connection,t_flag);

    int part = get_part(full_email);
    if(part == -1){
        return -1;
    }

    sprintf(command,"%s FETCH %d (BODY[1.%d])\r\n",get_tag(),messageNUM,part);
    n = send_command(connection,command,t_flag);
    n_check(n);
    char* mimed = get_full_email(connection,t_flag);
    fprintf(stdout,"%s\n",mimed);
    free(mimed);

    return 0;
}


int get_part(char* full_email){
    char* open,* close=NULL ; //"(" and ")"
    //int open_count = 0,close_count = 0;
    int body_count = 1;
    char* start = full_email;
    char* skip = strchr(start,'(');
    skip = strchr(skip+1,'(');
    //skip the IMAP bodystructure (
    start = skip+1;
    open = strchr(start,'(');
    if(open == NULL){
        return -1;
    }
    close = strchr(start,')');
    if(close == NULL){
        return -1;
    }   
    while (open != NULL && close != NULL)
    {

        if(open > close){
            char* part = strstr(start,"\"text\" \"plain\" ");
            if(part != NULL){
                                            
                if (part < close) {
                    const char* charset = strstr(part, "\"charset\" \"UTF-8\"");
                    if (charset != NULL && charset < close) {
                        return body_count;
                    }
                }
                else{
                    body_count ++;
                    start = close + 1;
                }
            }
            else{
                return -1;
            }
        }

        open = strchr(open+1,'(');
        if(open == NULL){
            break;
        }

        close = strchr(close+1,')');
        if(close == NULL){
            break;
        }
    }

    return -1;
}