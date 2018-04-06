/**
 *  \file   main.c
 *  \author James Petersen
 *
 *  \brief  This is the main file running the
 *          server and client.
 */

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "../../shared/base.h"
#include "../include/client.h"

/**
 *  main()
 *
 *  \brief  This is the main server loop function.
 */
int main(int argc, char **argv)
{
    int readfd;
    int writefd;
    mode_t mode = S_IRUSR | S_IWUSR;
    char recv_pipe[28];
    char send_pipe[25];
    snprintf(recv_pipe, 28, "./pipes/%d_receive", getpid());
    snprintf(send_pipe, 25, "./pipes/%d_send", getpid());

    printf("client receive pipe: %s\n", recv_pipe);
    printf("client receive pipe: %s\n", send_pipe);

#if 0

    if (mkfifo(recv_pipe, mode) < 0 && (errno != EEXIST))
    {
        fprintf(stderr, "Error creating %s FIFO\n", recv_pip);
        exit(ERROR);
    }

    if (mkfifo(SRV_WRITE, mode) < 0 && (errno != EEXIST))
    {
        fprintf(stderr, "Error creating server write FIFO\n");
        exit(ERROR);
    }
#endif
    struct message msg;
    int wfd = open(SRV_READ, O_WRONLY);
    msg.type = CONNECT;
    msg.pid  = getpid();
    printf("Attempting to connect to the server...\n");
    write(wfd, &msg, sizeof(struct message));

    close(wfd);

    exit(SUCCESS);
} /* function main */
