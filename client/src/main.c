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
    struct message msg;
    int wfd = open(SRV_READ, O_WRONLY);
    msg.type = CONNECT;
    msg.pid  = getpid();
    printf("Attempting to connect to the server...\n");
    write(wfd, &msg, sizeof(struct message));

    close(wfd);

    exit(SUCCESS);
} /* function main */
