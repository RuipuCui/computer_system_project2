#include "utils.h"

char* get_full_response(connection_t connection,int t_flag){
    int received_length = 0;
    int max_length = BUFFER_SIZE * 2;
    char* response = malloc(max_length);
    memset(response, 0, max_length);
    check_memory(response);
    //response[0] = '\0';

    char buffer[BUFFER_SIZE + 1];
    int n;

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        n = get_reply(connection,t_flag,buffer,BUFFER_SIZE);
        if (n <= 0) { 
            break;
        }
        
        buffer[n] = '\0';
        
        if ((strstr(buffer, "OK") || strstr(buffer, "BAD")) && strstr(buffer, "completed")) {
            memcpy(response + received_length, buffer, n); // Copy to response
            received_length += n;
            break; // Exit loop on completion signal
        }

        if (received_length + n > max_length) {
            max_length *= 2; // Double the buffer size if needed
            char* new_response = realloc(response, max_length);
            if (!new_response) {
                fprintf(stderr, "Memory allocation failed\n");
                free(response);
                return NULL;
            }
            response = new_response;
        }

        memcpy(response + received_length, buffer, n); // Copy buffer to response
        received_length += n;
    }

    response[received_length] = '\0'; // Ensure the final string is null-terminated
    return response;
}

char* get_full_email(connection_t connection,int t_flag) {

    char header[HEADER_BUFFER];
    memset(header,0,HEADER_BUFFER);
    char buffer[BUFFER_SIZE];
    int  total_length = 0;
    int n = get_reply(connection, t_flag,header, HEADER_BUFFER - 1);
    if(strstr(header,"NO") || strstr(header, "BAD")){
        memset(buffer,0,BUFFER_SIZE);
        fprintf(stdout,"Message not found\n");
        exit(3);
    }

    if (n <= 0) {
        fprintf(stderr, "Failed to read any data from server, or connection closed.\n");
        return NULL;
    }

    header[n] = '\0'; // Null-terminate the string

    int length = get_message_length(header);
    if (length <= 0) {
        fprintf(stderr, "Failed to determine message length from header.\n");

        return NULL;
    }

    char *full_response = malloc(sizeof(char) * (length+1)); // +1 for null terminator
    check_memory(full_response);
    memset(full_response,0,length);
    if (!full_response) {
        perror("Memory allocation failed");
        return NULL;
    }
    char *end = strchr(header, '}');
    if(end != NULL){
        end += 3; //skip the } and \r\n
    }

    //code to get data from the first buffer.
    int first_buffer_len = HEADER_BUFFER-(end-header);
    if(strlen(end) < length){
        memcpy(full_response,end,strlen(end));
    }
    else{
        memcpy(full_response,end,length);
        full_response[length] ='\0';
    }

    if(length < first_buffer_len){
        return full_response;
    }

    total_length = first_buffer_len;
    while (total_length < length) {
        memset(buffer, 0, BUFFER_SIZE);
        n = get_reply(connection,t_flag,buffer,BUFFER_SIZE);
        if (n < 0) {
            perror("Read failed");
            free(full_response);
            exit(3);
        } else if (n == 0) {
            fprintf(stderr, "Connection closed by server before complete data was received.\n");
            exit(3);
        }

        if (total_length + n > length){
            n = n-(total_length+n-length);
            buffer[n] = '\0';
        }
        
        memcpy(full_response+total_length-1,buffer,n);
        total_length += n;
    }
    
    if (total_length < length) {
        fprintf(stderr, "Incomplete response received.\n");
        full_response[total_length] = '\0';
    } else {
        full_response[length] = '\0'; // Ensure the string is null-terminated
    }

    return full_response;
}

//function to get length need for malloc
int get_message_length(char *header) {
    const char *start = strchr(header, '{');
    if (!start) return -1;

    start++; // Move past '{'
    const char *end = strchr(start, '}');
    if (!end) return -1;

    return atoi(start); // Directly convert the number in brackets to int
}


void replace_crlf_with_space(char* original) {
    int input_length = strlen(original);
    char* replaced = (char*)malloc(input_length + 1);
    check_memory(replaced);
    memset(replaced, 0, input_length + 1);
    char *p = original, *q = replaced;
    int whitenum = 0;
    while (*p != '\0') {
        if (*p == '\r' && *(p + 1) == '\n') {
            if (*(p + 2) == ' ') {
                *q++ = ' ';
                p += 3;
                whitenum ++;
            }else if(*(p + 2) == 9){
                *q++ = 9;
                p += 3;
            }else {
                *q++ = *p++;
                *q++ = *p++;
            }
        } else {
            *q++ = *p++;
        }
    }
    memcpy(original, replaced, q - replaced + 1);
    free(replaced);
}

void checkInvalidArgument(char* argu){
    char* c = argu;
    while(*c){
        if(*c == '\r' && *(c+1) == '\n'){
            exit(3);
        }
        c++;
    }
}

void check_memory(char* s){
    if (s == NULL){
        fprintf(stdout,"memory allocation fail");
        exit(5);
    }
}

void n_check(int n){
    if(n<=0){
        if(n<0){
            fprintf(stdout,"error during transmission");
            exit(3);
        }
        else{
            fprintf(stdout,"server close connection");
            exit(3);
        }
    }
}

char *my_strstr(const char *haystack, const char *needle) {
    if (*needle == '\0') {
        // If needle is an empty string, return the full haystack
        return (char *)haystack;
    }

    // Iterate through each character in haystack
    for (; *haystack != '\0'; haystack++) {
        const char *h = haystack;
        const char *n = needle;

        // Check substring starting at current haystack position
        while (*n != '\0' && *h == *n) {
            h++;
            n++;
        }

        // If end of needle reached (match found)
        if (*n == '\0') {
            return (char *)haystack;
        }
    }

    // If no match found
    return NULL;
}

void check_isdigit(char* s){
    char* c = s;
    while(*c){
        if(!isdigit(*c)){
            fprintf(stderr, "message number should be a number");
            exit(1);
        }
        c++;
    }
}