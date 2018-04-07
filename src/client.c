/**
 *  \file   client.c
 *  \author James Petersen
 *
 *  \brief  This is the main file running the client.
 */


#include "../include/base.h"
#include "../include/client.h"


static int request_connection(char *rpipe, char *wpipe)
{
    int wfd;
    struct connect_msg cmsg;
    int stat = SUCCESS;
    wfd = open(SRV_READ, O_WRONLY);
    cmsg.pid  = getpid();
    strncpy(cmsg.readp, rpipe, CLIENT_RD_NAME_SIZE);
    strncpy(cmsg.writep, wpipe, CLIENT_WR_NAME_SIZE);
    printf("Attempting to connect to the server...\n");
    if (write(wfd, &cmsg, sizeof(struct connect_msg)) < 0)
    {
        fprintf(stderr, "Failed to write connect message for PID: %d with errno: %d\n", getpid(), errno);
        stat = ERROR;
    }
    return stat;
}

/**
 *  main()
 *
 *  \brief  This is the main server loop function.
 */
int main(int argc, char **argv)
{
    char read_pipe[CLIENT_RD_NAME_SIZE];
    char write_pipe[CLIENT_WR_NAME_SIZE];
    mode_t mode;

    snprintf(read_pipe,  CLIENT_RD_NAME_SIZE, "./pipes/%d_read",  getpid());
    snprintf(write_pipe, CLIENT_WR_NAME_SIZE, "./pipes/%d_write", getpid());

    printf("client receive pipe: %s\n", read_pipe);
    printf("client write pipe:   %s\n", write_pipe);
    
    mode = S_IRUSR | S_IWUSR;

    if (mkfifo(read_pipe, mode) < 0 && (errno != EEXIST))
    {
        fprintf(stderr, "Error creating %s FIFO\n", read_pipe);
        exit(ERROR);
    }

    if (mkfifo(write_pipe, mode) < 0 && (errno != EEXIST))
    {
        fprintf(stderr, "Error creating %s FIFO\n", write_pipe);
        exit(ERROR);
    }

    if (request_connection(read_pipe, write_pipe) != SUCCESS)
    {
        fprintf(stderr, "Connection request failed for PID: %d\n", getpid());
        exit(ERROR);
    }
    else
        printf("Connected to the server.\n");

    while (1);
    exit(SUCCESS);
} /* function main */
