/**
 *  \file   server.c
 *  \author James Petersen
 *
 *  \brief  This is the main file running the server.
 */

#include "../include/server.h"
#include "../../shared/base.h"

/**
 *  strip_newline macro, strips newline off incoming message.
 */
#define strip_newline(str) str[strlen(str)-1] = '\0'


/**
 *  server_handle()
 *
 *  \brief  This is the server function that reads
 *          from the pipe.
 */
int server_handle(const pid_t child_pid)
{
    printf("I'm the server with child: %d\n", child_pid);
    exit(SUCCESS);
} /* function server_handle */
