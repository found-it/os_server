/**
 *  \file   server.c
 *  \author James Petersen
 *
 *  \brief  This is the main file running the server.
 */


#include "../include/base.h"
#include "../include/server.h"


void print_client(struct connect_msg client)
{
    printf("PID: %d\n", client.pid);
    printf("RP:  %s\n", client.readp);
    printf("WP:  %s\n", client.writep);
}


static int client_handle(struct connect_msg client)
{
    print_client(client);
    while (1);
    return SUCCESS;
}


/**
 *  main()
 *
 *  \brief  This is the main server loop function.
 */
int main(int argc, char **argv)
{
    pid_t pid;
    int readfd;
    int bytes;
    struct connect_msg conn_msg;
    mode_t mode = S_IRUSR | S_IWUSR;

    if (mkfifo(SRV_READ, mode) < 0 && (errno != EEXIST))
    {
        fprintf(stderr, "Error creating server read FIFO\n");
        exit(ERROR);
    }

    readfd = open(SRV_READ, O_RDONLY);
    memset(&conn_msg, '\0', sizeof(struct connect_msg));

    while ((bytes = read(readfd, &conn_msg, sizeof(struct connect_msg))) > 0)
    {
        printf("Read in %d bytes\n", bytes);
        printf("Client %d is trying to connect\n", conn_msg.pid);
        printf("Read pipe:  %s\n", conn_msg.readp);
        printf("Write pipe: %s\n", conn_msg.writep);
        
        if ((pid = fork()) == 0)
        {
            client_handle(conn_msg);
        }
        

        memset(&conn_msg, '\0', sizeof(struct connect_msg));
    }

#if 0
    switch (pid = fork())
    {
        case -1:    /* error */
            fprintf(stderr, "Error forking\n");
            exit(ERROR);
            break;

        case 0:     /* child */
            printf("I'm the child\n");
            break;
        
        default:    /* parent */
            server_handle(pid);
            break;
    }
#endif

    close(readfd);
//    close(writefd);
    exit(SUCCESS);
} /* function main */
