/**
 *  \file   server.c
 *  \author James Petersen
 *
 *  \brief  This is the main file running the server.
 */


#include "../include/base.h"
#include "../include/server.h"
#include "../include/messages.h"

static int send_time(const int fd)
{
    time_t raw;
    struct tm *timeinfo;
    time(&raw);
    timeinfo = localtime(&raw);

    return send_pipe_msg(fd, RESPONSE, TIME, timeinfo);
}

/**
 *
 */
static int exec_command(const int fd[], const int cmd)
{
    switch (cmd)
    {
        case TIME:
            send_time(fd[WRITE]);
            break;
        case STATUS:
            send_pipe_msg(fd[WRITE], RESPONSE, STATUS, "Here is the status");
            break;
        case EXIT:
            return TRUE;
            break;
        default:
            break;
    }
    return FALSE;
}




/**
 *
 */
static int client_handle(struct connect_msg client)
{
    struct pipe_msg msg;
    int bytes;
    int exit = FALSE;
    int fd[2];

    /* open up the server-read / client-write FIFO */
    if ((fd[READ] = open(client.writep, O_RDONLY)) < 0)
    {
        fprintf(stderr, "Error opening %s in func: %s\n", client.writep, __func__);
        _exit(ERROR);
    }

    /* open up the server-write / client-read FIFO */
    if ((fd[WRITE] = open(client.readp, O_WRONLY)) < 0)
    {
        fprintf(stderr, "Error opening %s in func: %s\n", client.readp, __func__);
        _exit(ERROR);
    }

    /* main loop */
    while ((bytes = read(fd[READ], &msg, sizeof(struct pipe_msg))) > 0)
    {
        /* parse by message type */
        switch (msg.type)
        {
            case COMMAND:
                exit = exec_command(fd, msg.cmd);
                break;
            case TEXT:
                printf("msg: %s\n", msg.body.text);
                break;
            default:
                break;
        }
        
        /* check exit flag */
        if (exit == TRUE)
            break;
    }

    /* cleanup */
    close(fd[WRITE]);
    close(fd[READ]);
    _exit(SUCCESS);
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

    /* make the server FIFO */
    if (mkfifo(SRV_READ, mode) < 0 && (errno != EEXIST))
    {
        fprintf(stderr, "Error creating server read FIFO\n");
        exit(ERROR);
    }

    /* open up the server FIFO */
    readfd = open(SRV_READ, O_RDONLY);
    memset(&conn_msg, '\0', sizeof(struct connect_msg));

    while (1)
    {
        bytes = read(readfd, &conn_msg, sizeof(struct connect_msg));
        if ((pid = fork()) == 0)
            client_handle(conn_msg);
        
        memset(&conn_msg, '\0', sizeof(struct connect_msg));
    }

    /* cleanup */
    close(readfd);
    exit(SUCCESS);
} /* function main */
