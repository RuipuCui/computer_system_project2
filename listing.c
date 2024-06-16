#include "listing.h"


int listing(connection_t connection,int t_flag){
    char command[BUFFER_SIZE];
    int n;
    sprintf(command, "A003 FETCH 1:* (BODY.PEEK[HEADER.FIELDS (SUBJECT)])\r\n");
    n = send_command(connection,command,t_flag);
    n_check(n);
    char* reply = get_full_response(connection,t_flag);
    int i = 0;
    char* current = reply;
    int length;
    int total_length = strlen(reply);
    while (i < total_length)
    {

        char* end = strchr(current,'}');
        if(end == NULL){
            break;
        }
        else{
            end += 3;
            }
        length = get_message_length(current);
        if(length == -1){
            break;
        }
        if(length == 2){
            fprintf(stdout,"%d: %s",get_count(),"<No subject>\n");
            current = end + 3;
            continue;
        }
        else{
            length -= strlen("Subject:");
        }

        end += strlen("Subject:");
        end[length-4] = '\0';
        replace_crlf_with_space(end);
        fprintf(stdout,"%d:%s\n",get_count(),end);
        end += length;
        current = end + 1;
    }
    free(reply);
    return 0;
}   

int get_count(){
    static int index = 0;
    index++;
    return index;

}

