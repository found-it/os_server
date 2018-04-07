/**
 *  \file   server.c
 *  \author James Petersen
 *
 *  \brief  This is the main file running the server.
 */


#include "../include/base.h"
#include "../include/server.h"


void print_client(struct connect_msg client)
{
    printf("PID: %d\n", client.pid);
    printf("RP:  %s\n", client.readp);
    printf("WP:  %s\n", client.writep);
}

static int exec_command(const int fd[], const int cmd)
{
    struct pipe_msg msg;
    switch (cmd)
    {
        case TIME:
            msg.type = TEXT;
            msg.cmd  = NA;
            strncpy(msg.text, "Here is the time\n", MAX_LEN);
            write(fd[WRITE], &msg, sizeof(struct pipe_msg));
            break;
        case STATUS:
            msg.type = TEXT;
            msg.cmd  = NA;
            strncpy(msg.text, "Here is the status\n", MAX_LEN);
            write(fd[WRITE], &msg, sizeof(struct pipe_msg));
            break;
        case EXIT:
            printf(RED "Server (PID: %d) exits\n" RESET, getpid());
            return TRUE;
            break;
        default:
            break;
    }
    return SUCCESS;

}

static int client_handle(struct connect_msg client)
{
    print_client(client);
    struct pipe_msg msg;
    int bytes;
    int exit = 0;
    int fd[2];

    printf("Trying to open %s.\n", client.readp);
    if ((fd[WRITE] = open(client.readp, O_WRONLY)) < 0)
    {
        fprintf(stderr, "Error opening %s in func: %s\n", client.readp, __func__);
        _exit(ERROR);
    }
    printf("Opened %s.\n", client.readp);

    printf("Trying to open %s.\n", client.writep);
    if ((fd[READ] = open(client.writep, O_RDONLY)) < 0)
    {
        fprintf(stderr, "Error opening %s in func: %s\n", client.writep, __func__);
        _exit(ERROR);
    }
    printf("Opening %s.\n", client.writep);

    printf("Trying to read from %s\n", client.writep);
    while ((bytes = read(fd[READ], &msg, sizeof(struct pipe_msg))) > 0)
    {
        switch (msg.type)
        {
            case COMMAND:
                exit = exec_command(fd, msg.cmd);
                break;
            case TEXT:
                printf("msg: %s\n", msg.text);
                break;
            default:
                break;
        }
        
        if (exit == TRUE)
            break;
    }
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

    if (mkfifo(SRV_READ, mode) < 0 && (errno != EEXIST))
    {
        fprintf(stderr, "Error creating server read FIFO\n");
        exit(ERROR);
    }

    readfd = open(SRV_READ, O_RDONLY);
    memset(&conn_msg, '\0', sizeof(struct connect_msg));

    while (1)//(bytes = read(readfd, &conn_msg, sizeof(struct connect_msg))) > 0)
    {
        bytes = read(readfd, &conn_msg, sizeof(struct connect_msg));
        printf("Read in %d bytes\n", bytes);
        printf("Client %d is trying to connect\n", conn_msg.pid);
        printf("Read pipe:  %s\n", conn_msg.readp);
        printf("Write pipe: %s\n", conn_msg.writep);
        
        if ((pid = fork()) == 0)
        {
            printf("Branching off for client\n");
            client_handle(conn_msg);
        }
        
        printf("After client handle is called\n");

        memset(&conn_msg, '\0', sizeof(struct connect_msg));
    }
    printf("Broke out of the while loop.\n");

    close(readfd);
    exit(SUCCESS);
} /* function main */
