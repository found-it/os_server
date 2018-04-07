
/**
 *  \file   messages.h
 *  \author James Petersen
 *
 *  \brief  
 */

#ifndef MESSAGES_H
#define MESSAGES_H

#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

/**
 *  Message type
 */
enum msg_type
{
    TEXT     = 0,
    COMMAND  = 1,
    RESPONSE = 2
};

/**
 *  Command type
 */
enum cmd_type
{
    NA     = 0,
    TIME   = 1,
    STATUS = 2,
    EXIT   = 3
};


union msg_body
{
    char text[MAX_LEN];
    struct tm time;
};



/**
 *  Message struct
 */
struct pipe_msg
{
    enum msg_type type;
    enum cmd_type cmd;
    union msg_body body;
};

struct connect_msg
{
    pid_t pid;
    char readp[CLIENT_RD_NAME_SIZE];
    char writep[CLIENT_WR_NAME_SIZE];
};

int send_pipe_msg(int fd, int type, int cmd, void *text);

#endif
