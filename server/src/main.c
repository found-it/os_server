/**
 *  \file   main.c
 *  \author James Petersen
 *
 *  \brief  This is the main file running the
 *          server and client.
 */

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../../shared/base.h"
#include "../include/server.h"


/**
 *  main()
 *
 *  \brief  This is the main server loop function.
 */
int main(int argc, char **argv)
{
    pid_t pid;
    int readfd;
    int writefd;
    int bytes;
    struct message client_msg;
    mode_t mode = S_IRUSR | S_IWUSR;

    if (mkfifo(SRV_READ, mode) < 0 && (errno != EEXIST))
    {
        fprintf(stderr, "Error creating server read FIFO\n");
        exit(ERROR);
    }

    if (mkfifo(SRV_WRITE, mode) < 0 && (errno != EEXIST))
    {
        fprintf(stderr, "Error creating server write FIFO\n");
        exit(ERROR);
    }

    readfd = open(SRV_READ, O_RDONLY);
    memset(&client_msg, '\0', sizeof(struct message));

    while ((bytes = read(readfd, &client_msg, sizeof(struct message))) > 0)
    {
        printf("Read in %d bytes\n", bytes);
        printf("Server read: %s\n", client_msg.text);
        if (client_msg.type == CONNECT)
            printf("Client %d is trying to connect\n", client_msg.pid);
        memset(&client_msg, '\0', sizeof(struct message));
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
