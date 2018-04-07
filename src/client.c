/**
 *  \file   client.c
 *  \author James Petersen
 *
 *  \brief  This is the main file running the client.
 */


#include "../include/base.h"
#include "../include/client.h"


enum commands
{
    Send    = 0,
    Help    = 1,
    Enter   = 2,
    Exit    = 3,
    Status  = 4,
    Time    = 5
};




/**
 *
 */
static int request_connection(const char *pipes[])
{
    int wfd;
    struct connect_msg cmsg;
    int stat = SUCCESS;

    wfd = open(SRV_READ, O_WRONLY);

    cmsg.pid  = getpid();
    strncpy(cmsg.readp,  pipes[READ], CLIENT_RD_NAME_SIZE);
    strncpy(cmsg.writep, pipes[WRITE], CLIENT_WR_NAME_SIZE);

    printf("Attempting to connect to the server...\n");
    if (write(wfd, &cmsg, sizeof(struct connect_msg)) < 0)
    {
        fprintf(stderr, "Failed to write connect message for PID: %d with errno: %d\n", getpid(), errno);
        stat = ERROR;
    }
    close(wfd);
    return stat;
}




/**
 *
 */
static int parse_command(const char *input)
{
    if (strncmp(input, "send:", 5) == 0)        /* send command */
        return Send;

    else if (strncmp(input, "exit", 4) == 0)    /* exit command */
        return Exit;

    else if (strncmp(input, "help", 4) == 0)    /* help command */
        return Help;

    else if (strncmp(input, "\n", 1) == 0)      /* ignore stray <return> */
        return Enter;

    else if (strncmp(input, "status", 6) == 0)
        return Status;

    else if (strncmp(input, "time", 4) == 0)
        return Time;

    else                                        /* incorrect command */
        return ERROR;
}


/**
 *
 */
static int exec_command(const int fd[], const int cmd, char *input)
{
    int exit = 0;
    struct pipe_msg msg;
    struct pipe_msg in_msg;
    switch (cmd)
    {
        case Send:
            printf("Send\n");
            /* strip off 'send:' */
            memmove(input, input + 5, MAX_LEN - 5);

            msg.type = TEXT;
            msg.cmd  = NA;
            strncpy(msg.text, input, MAX_LEN);

            write(fd[WRITE], &msg, sizeof(struct pipe_msg));
            break;
        case Exit:
            printf(RED "Client (PID: %d) exits\n" RESET, getpid());
            msg.type = COMMAND;
            msg.cmd  = EXIT;
            write(fd[WRITE], &msg, sizeof(struct pipe_msg));
            exit = TRUE;
            break;
        case Help:
            printf("usage\n");
            break;
        case Enter:
            /* do nothing */
            break;
        case Status:
            printf("Status\n");
            msg.type = COMMAND;
            msg.cmd  = STATUS;
            write(fd[WRITE], &msg, sizeof(struct pipe_msg));

            read(fd[READ], &in_msg, sizeof(struct pipe_msg));
            printf("Status: %s\n", in_msg.text);

            break;
        case Time:
            printf("Time\n");
            msg.type = COMMAND;
            msg.cmd  = TIME;
            write(fd[WRITE], &msg, sizeof(struct pipe_msg));

            read(fd[READ], &in_msg, sizeof(struct pipe_msg));
            printf("Status: %s\n", in_msg.text);

            break;
        default:
            printf("Incorrect command. Type 'help' for list of commands.\n");
            break;
    }
    return exit;
}



/**
 *
 */
static int client(const char *pipes[])
{
    printf("in %s\n", __func__);
    char input[MAX_LEN];
    int fd[2];
    printf("Trying to open %s.\n", pipes[WRITE]);
    if ((fd[WRITE] = open(pipes[WRITE], O_WRONLY)) < 0)
    {
        fprintf(stderr, "Error opening %s in func: %s\n", pipes[WRITE], __func__);
        return ERROR;
    }
    printf("Opened %s.\n", pipes[WRITE]);

    printf("Trying to open %s.\n", pipes[READ]);
    if ((fd[READ] = open(pipes[READ], O_RDONLY)) < 0)
    {
        fprintf(stderr, "Error opening %s in func: %s\n", pipes[READ], __func__);
        return ERROR;
    }
    printf("Opened %s.\n", pipes[READ]);

    /* main client loop */
    int exit = 0;
    int cmd;
    while (exit != TRUE)
    {
        /* prompt user */
        printf(PINK "client" BLUE "@" GREEN "pid-%d" RESET "$ ", getpid());

        /* get the input */
        memset(input, '\0', sizeof(char)*MAX_LEN);
        fgets(input, MAX_LEN-1, stdin);

        /* parse command */

        cmd = parse_command(input);
        exit = exec_command(fd, cmd, input);
    }
    close(fd[WRITE]);
    close(fd[READ]);
    return SUCCESS;
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
    const char *pipes[2];
    mode_t mode;

    snprintf(read_pipe,  CLIENT_RD_NAME_SIZE, "./pipes/%d_read",  getpid());
    snprintf(write_pipe, CLIENT_WR_NAME_SIZE, "./pipes/%d_write", getpid());

    pipes[READ]  = read_pipe;
    pipes[WRITE] = write_pipe;

    mode = S_IRUSR | S_IWUSR;

    if (mkfifo(pipes[READ], mode) < 0 && (errno != EEXIST))
    {
        fprintf(stderr, "Error creating %s FIFO\n", pipes[READ]);
        exit(ERROR);
    }

    if (mkfifo(pipes[WRITE], mode) < 0 && (errno != EEXIST))
    {
        fprintf(stderr, "Error creating %s FIFO\n", write_pipe);
        exit(ERROR);
    }

    if (request_connection(pipes) != SUCCESS)
    {
        fprintf(stderr, "Connection request failed for PID: %d\n", getpid());
        exit(ERROR);
    }
    else
        printf("Connected to the server.\n");

    client(pipes);

    exit(SUCCESS);
} /* function main */
