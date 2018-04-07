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

    /* set the flags */
    msg.flags.type = type;
    msg.flags.cmd  = cmd;

    /* fill the body of the message */
    if (type == TEXT)                                   /* text */
        strncpy(msg.body.text, (char*)body, MAX_LEN);

    else if (type == RESPONSE && cmd == TIME)           /* time response */
        msg.body.time = *(struct tm *)body;

    else if (type == RESPONSE && cmd == STATUS)         /* status response */
        msg.body.status = *(int *)body;

    /* send the message */
    if (write(fd, &msg, sizeof(msg)) < 0)
    {
        fprintf(stderr, "Error sending pipe_msg through pipe.\n");
        return ERROR;
    }
    return SUCCESS;
}

