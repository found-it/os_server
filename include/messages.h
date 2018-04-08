/**
 *  \file   messages.h
 *  \author James Petersen
 *
 *  \brief  Describes the message parts and breakdown.
 */

#ifndef MESSAGES_H
#define MESSAGES_H

#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

/* Max buffer length */
#define MAX_LEN  100

/**
 *  Message type flag
 */
enum msg_type
{
    TEXT     = 0,
    COMMAND  = 1,
    RESPONSE = 2
};


/**
 *  Command type flag
 */
enum cmd_type
{
    NA     = 0,
    TIME   = 1,
    STATUS = 2,
    EXIT   = 3
};


/**
 *  Message flags
 */
struct msg_flags
{
    enum msg_type type;
    enum cmd_type cmd;
};


/**
 *  Body of the message.
 */
union msg_body
{
    char text[MAX_LEN]; /* text for SEND */
    struct tm time;     /* time for TIME */
    int status;         /* status for STATUS */
};


/**
 *  Main message struct, used to talk between processes
 */
struct pipe_msg
{
    struct msg_flags flags;
    union  msg_body  body;
};


/**
 *  Connection message, sends the PID of the client and
 *  its read/write FIFO's.
 */
struct connect_msg
{
    pid_t pid;
};


/**
 *  send_pipe_msg()
 *
 *  \brief  Fills a pipe_msg with the appropriate information
 *          and sends it to the pipe described by fd.
 */
int send_pipe_msg(int fd, int type, int cmd, void *text);

#endif
