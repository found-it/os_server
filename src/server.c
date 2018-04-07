/**
 *  \file   server.c
 *  \author James Petersen
 *
 *  \brief  This is the main file running the server.
 */


#include "../include/base.h"
#include "../include/server.h"
#include "../include/messages.h"


volatile static int _nclients = 0;
volatile static int _shutdown = 0;


static void handle_signal(int sig)
{
    if (sig == SIGCHLD)
        --_nclients;

    else if (sig == SIGINT)
        _shutdown = 1;
}



/**
 *
 */
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
            send_pipe_msg(fd[WRITE], RESPONSE, STATUS, (int *)&_nclients);
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
        switch (msg.flags.type)
        {
            case COMMAND:
                exit = exec_command(fd, msg.flags.cmd);
                break;
            case TEXT:
                printf("Client %d says \"" TEAL "%s" RESET "\"\n", client.pid, msg.body.text);
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
    pid_t  pid;
    mode_t mode;
    int    bytes;
    int    srv_fd;
    struct connect_msg conn_msg;
    struct sigaction sigact;

    /* set up signal handling */
    sigact.sa_handler = handle_signal;
    sigaction(SIGCHLD, &sigact, NULL);

    /* set the FIFO permissions */
    mode = S_IRUSR | S_IWUSR;

    /* make the server FIFO */
    if (mkfifo(SRV_READ, mode) < 0 && (errno != EEXIST))
    {
        fprintf(stderr, "Error creating server read FIFO\n");
        exit(ERROR);
    }

    /* open up the server FIFO */
    srv_fd = open(SRV_READ, O_RDONLY);
    memset(&conn_msg, '\0', sizeof(struct connect_msg));

    while (!_shutdown)
    {
        bytes = read(srv_fd, &conn_msg, sizeof(struct connect_msg));
        if (bytes > 0)
        {
            ++_nclients;
            pid = fork();
            if (pid < 0)
            {
                fprintf(stderr, "Error fork failed!\n");
                _shutdown = 1;
            }
            else if (pid == 0)
                client_handle(conn_msg);
        }

        memset(&conn_msg, '\0', sizeof(struct connect_msg));
    }

    printf("Exiting gracefully..\n");
    /* cleanup */
    close(srv_fd);
    exit(SUCCESS);
} /* function main */
