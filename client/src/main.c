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
    const char swrite[] = "/tmp/server_read";
    int wfd = open(swrite, O_WRONLY);

#if 0
    char buf[MAX_LEN];
    fgets(buf, MAX_LEN, stdin);
    strcpy(msg.text, buf);
#endif

    msg.type = CONNECT;
    msg.pid  = getpid();
    msg.text = NULL;

    printf("writing: %s\n");
    write(wfd, &msg, sizeof(struct message));

    close(wfd);

    exit(SUCCESS);
} /* function main */
