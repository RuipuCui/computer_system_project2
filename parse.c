#include "parse.h"

int parse(connection_t connection,int t_flag,int messageNum){

    char command[BUFFER_SIZE];
    int n;
    sprintf(command,"%s FETCH %d BODY.PEEK[HEADER.FIELDS (FROM)]\r\n",get_tag(),messageNum);
    n = send_command(connection,command,t_flag);
    n_check(n);
    char* from = get_full_email(connection,t_flag);
    replace_crlf_with_space(from);
    header_format(from);
    fprintf(stdout,"%s\n",from);
    free(from);

    sprintf(command,"%s FETCH %d BODY.PEEK[HEADER.FIELDS (TO)]\r\n",get_tag(),messageNum);
    n = send_command(connection,command,t_flag);
    
    char* to = get_full_email(connection,t_flag);
    if(strspn(to,"\r\n")){
        fprintf(stdout, "To:\n");
    }
    else{
        replace_crlf_with_space(to);
        header_format(to); 
        fprintf(stdout,"%s\n",to);
    }
    free(to);

    sprintf(command,"%s FETCH %d BODY.PEEK[HEADER.FIELDS (DATE)]\r\n",get_tag(),messageNum);
    n = send_command(connection,command,t_flag);
    
    char* date = get_full_email(connection,t_flag);
    replace_crlf_with_space(date);
    header_format(date);
    fprintf(stdout,"%s\n",date);


    sprintf(command,"%s FETCH %d BODY.PEEK[HEADER.FIELDS (SUBJECT)]\r\n",get_tag(),messageNum);
    n = send_command(connection,command,t_flag);
    
    char* subject = get_full_email(connection,t_flag);
    if(strspn(to,"\r\n")){
        fprintf(stdout, "Subject: <No subject>\n");
    }
    else{
        replace_crlf_with_space(subject);
        header_format(subject);
        fprintf(stdout,"%s\n",subject);

    }
    free(subject);
    
    return 0;
}

void header_format(char* s){
    if(!s){
        return;
    }
    int i = 0;
    while(s[i] != ':'){
        if(i == 0){
            if(s[i] >= 97){
                s[i] -= 32;
            }
        }else{
            if(s[i] < 97){
                s[i] += 32;
            }
        }
        i++;
    }

    while(s[i]){
        if(s[i] == 10 || s[i] == 13){
            s[i] = 0;
        }
        i++;
    }
}




