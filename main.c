#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "socket.h"
#include "utils.h"

int main(int argc, char* argv[]) {
    if(argc < 7){
        exit(1);
    }
    char *username = NULL;
    char *password = NULL;
    char *folder = NULL;
    int messageNum = -1;
    int t_flag = 0;
    char *command = NULL;
    char *server_name = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-u") == 0 && i + 1 < argc) {
            checkInvalidArgument(argv[i+1]);
            username = argv[++i];
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            checkInvalidArgument(argv[i+1]);
            password = argv[++i];
        } else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            checkInvalidArgument(argv[i+1]);
            folder = argv[++i];
        } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            checkInvalidArgument(argv[i+1]);
            check_isdigit(argv[i+1]);
            messageNum = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-t") == 0) {
            t_flag = 1;
        } else {
            // Handle non-option arguments
            if (!command) {
                checkInvalidArgument(argv[i]);
                command = argv[i];  // First non-option argument is the command
            } else if (!server_name) {
                checkInvalidArgument(argv[i]);
                server_name = argv[i];  // Second non-option argument is the server name
            } else {
                exit(1);
            }
        }
    }

    int result = set_connection(server_name,username,password,folder,command,messageNum, t_flag);

    if(result){
    return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
