/**
 *  \file   server.c
 *  \author James Petersen
 *
 *  \brief  This is the main file running the server.
 */


#include "../include/base.h"
#include "../include/server.h"
#include "../include/messages.h"


/* number of clients */
volatile static int _nclients = 0;

/* loop control variable */
volatile static int _shutdown = 0;


/**
 *  handle_signal()
 *
 *  \brief  Handles the signals created by 
 *          the child processes.
 */
static void handle_signal(int sig)
{
    if (sig == SIGCHLD)
        --_nclients;

    else if (sig == SIGINT)
        _shutdown = 1;
}


/**
 *  send_time()
 *
 *  \brief  Gets the current time and sends 
 *          it to the requesting client.
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
 *  exec_command()
 *
 *  \brief  Executes the command sent by the client.
 */
static int exec_command(const int fd[], const int cmd)
{
    int exit = FALSE;
    switch (cmd)
    {
        case TIME:
            send_time(fd[WRITE]);
            break;

        case STATUS:
            send_pipe_msg(fd[WRITE], RESPONSE, STATUS, (int *)(&_nclients));
            break;

        case EXIT:
            exit = TRUE;
            break;

        default:
            fprintf(stderr, "Command(%d) not recognized.\n", cmd);
            break;
    }
    return FALSE;
}


/**
 *  client_handle()
 *
 *  \brief  Main handler for the client process.
 */
static int client_handle(const struct connect_msg client)
{
    int  bytes;
    int  fd[2];
    int  exit = FALSE;
    char srv_read[CLIENT_PIPE_NAME_SIZE];
    char srv_write[CLIENT_PIPE_NAME_SIZE];
    struct pipe_msg msg;

    /* create the FIFO name strings */
    snprintf(srv_write, CLIENT_PIPE_NAME_SIZE, "./pipes/%d_read",  client.pid);
    snprintf(srv_read,  CLIENT_PIPE_NAME_SIZE, "./pipes/%d_write", client.pid);

    /* open up the server-read / client-write FIFO */
    if ((fd[READ] = open(srv_read, O_RDONLY)) < 0)
    {
        fprintf(stderr, "Error opening %s in func: %s\n", srv_read, __func__);
        _exit(ERROR);
    }

    /* open up the server-write / client-read FIFO */
    if ((fd[WRITE] = open(srv_write, O_WRONLY)) < 0)
    {
        fprintf(stderr, "Error opening %s in func: %s\n", srv_write, __func__);
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
int main(void)
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
    if (mkfifo(SRV_FIFO, mode) < 0 && (errno != EEXIST))
    {
        fprintf(stderr, "Error creating server read FIFO\n");
        exit(ERROR);
    }

    /* open up the server FIFO */
    srv_fd = open(SRV_FIFO, O_RDONLY);
    memset(&conn_msg, '\0', sizeof(struct connect_msg));

    /* listen for new client connections */
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

    /* cleanup */
    close(srv_fd);
    exit(SUCCESS);
}
