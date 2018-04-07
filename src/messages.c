/**
 *  \file   messages.c
 *  \author James Petersen
 *
 *  \brief  
 */

#include "../include/base.h"
#include "../include/messages.h"


int send_pipe_msg(int fd, int type, int cmd, void *body)
{
    struct pipe_msg msg;
    msg.type = type;
    msg.cmd  = cmd;
    if (type == TEXT)
        strncpy(msg.body.text, (char*)body, MAX_LEN);

    else if (type == RESPONSE && cmd == TIME)
        msg.body.time = *(struct tm*)body;
    
    else if (type == RESPONSE && cmd == STATUS)
        strncpy(msg.body.text, (char*)body, MAX_LEN);

    if (write(fd, &msg, sizeof(msg)) < 0)
    {
        fprintf(stderr, "Error sending pipe_msg through pipe.\n");
        return ERROR;
    }
    return SUCCESS;
}

