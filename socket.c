#include "socket.h"

void initialize_ssl() {
    SSL_load_error_strings();   
    OpenSSL_add_ssl_algorithms();
}

SSL_CTX* create_ssl_context() {
    const SSL_METHOD *method = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    return ctx;
}

int set_connection(char *server, char *username, char *password, char *folder, char *task, int messageNUM, int t_flag)
{

    if(task == NULL || server == NULL){
        fprintf(stdout,"Login failure\n");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    char command[BUFFER_SIZE];
    connection_t connection;
    connection.connfd = 0;
    connection.ssl = NULL;
    int connfd = -1, s, n;
    struct addrinfo hints, *res, *rp;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    if (!t_flag)
    {
        s = getaddrinfo(server, PORT, &hints, &res);
    }
    else
    {
        s = getaddrinfo(server, TSL_PORT, &hints, &res);
    }
    if (s != 0)
    {
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        if (!t_flag)
        {

            s = getaddrinfo(server, PORT, &hints, &res);
        }
        else
        {
            s = getaddrinfo(server, TSL_PORT, &hints, &res);
        }
        if (s != 0)
        {
            fprintf(stdout,"Login failure");
            exit(1);
        }
    }

    if (t_flag)
    {
        initialize_ssl();
        SSL_CTX *ctx = create_ssl_context();
        for (rp = res; rp != NULL; rp = rp->ai_next)
        {
            connfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (connfd == -1)
                continue;

            if (connect(connfd, rp->ai_addr, rp->ai_addrlen) != -1)
            {
                SSL *ssl = SSL_new(ctx);
                SSL_set_fd(ssl, connfd);

                if (SSL_connect(ssl) != 1)
                { // Perform the SSL/TLS handshake
                    ERR_print_errors_fp(stderr);
                    SSL_free(ssl);
                    continue;
                }
                connection.ssl = ssl;
                break; 
            }
            close(connfd);
            connfd = -1;
            t_flag = 0; 
        }

        SSL_CTX_free(ctx);
    }

    if (!t_flag)
    {
        for (rp = res; rp != NULL; rp = rp->ai_next)
        {
            connfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (connfd == -1)
                continue;
            if (connect(connfd, rp->ai_addr, rp->ai_addrlen) != -1)
            {
                connection.connfd = connfd;
                break;
            }
            close(connfd);
            connfd = -1;
        }
    }
    freeaddrinfo(res);
    if(connfd ==-1){
        fprintf(stdout,"Login failure\n");
        exit(3);
    }
    n = get_reply(connection,t_flag,buffer,BUFFER_SIZE);
    if(n <= 0){
        fprintf(stderr,"error occured\n");
        exit(3);
    }
   
    sprintf(command, "%s LOGIN %s %s\r\n", get_tag(), username, password);
    n = send_command(connection, command, t_flag);
    if(n <= 0){
        fprintf(stderr,"error occured\n");
        exit(3);
    }

    n = get_reply(connection,t_flag,buffer,BUFFER_SIZE);
    if(n <= 0){
        fprintf(stderr,"error occured\n");
        exit(2);
    }

    if (strstr(buffer, "failed") != NULL)
    {
        fprintf(stdout, "Login failure\n");
        exit(3);
    }
    clear_server_buffer(connection,t_flag);
    memset(buffer, 0, BUFFER_SIZE);

    if (folder == NULL)
    {
        folder = "Inbox";
    }
    sprintf(command, "%s SELECT \"%s\"\r\n", get_tag(), folder);
    n = send_command(connection, command, t_flag);
    n_check(n);

    clear_server_buffer(connection,t_flag);
    memset(buffer, 0, BUFFER_SIZE);

    if (messageNUM == -1)
    {
        messageNUM = '*';
    }
    int result = 0;
    if (!strcmp(task, RETRIEVE))
    {
        result = retrieving(connection, t_flag, messageNUM);
    }

    if (!strcmp(task, PARSE))
    {

        result = parse(connection, t_flag, messageNUM);
    }

    if (!strcmp(task, MIME))
    {

        result = mime(connection, t_flag, messageNUM);
    }

    if (!strcmp(task, LIST))
    {

        result = listing(connection, t_flag);
    }


    if(t_flag){
        SSL_shutdown(connection.ssl);
        SSL_free(connection.ssl);
    }

    close(connection.connfd);
    return result;
}

const char *get_tag()
{
    static int count = 1;
    static char tag[10];

    sprintf(tag, "A%d", count++);
    return tag;
}

int send_command(connection_t connection, const char *command, int flag)
{
    static int count = 0;
    count ++;

    if (!flag)
    {
        return write(connection.connfd, command, strlen(command));
    }
    else{
        return SSL_write(connection.ssl, command, strlen(command));
    }
}

int get_reply(connection_t connection, int flag, char *data, int data_size)
{
    int n;
    memset(data, 0, data_size);
    if (!flag)
    {
        memset(data, 0, data_size);
        n = read(connection.connfd, data, data_size);
        data[data_size] = 0;
    }
    else
    {
        memset(data, 0, data_size);
        n = SSL_read(connection.ssl, data, data_size);
        data[data_size] = 0;
    }
    return n;
}



void clear_server_buffer(connection_t connection,int t_flag)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);
    //buffer[0] = 0;
    int n;  //
    while ((n = get_reply(connection,t_flag, buffer, BUFFER_SIZE)) >= 0 && !my_strstr(buffer, "completed"))
    {

        if (my_strstr(buffer, "Logged in"))
        {
            memset(buffer, 0, BUFFER_SIZE);
            return;
        }
        if (my_strstr(buffer, "NO") || my_strstr(buffer, "BAD"))
        {
            memset(buffer, 0, BUFFER_SIZE);
            fprintf(stdout, "Folder not found\n");
            exit(3);

        }
        memset(buffer, 0, BUFFER_SIZE);
        //buffer[0] = 0;
        continue;
    }
    n_check(n);
    
    memset(buffer, 0, BUFFER_SIZE);
    return;
}



