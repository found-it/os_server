/**
 *  \file   client.c
 *  \author James Petersen
 *
 *  \brief  This is the main file running the client.
 */


#include "../include/base.h"
#include "../include/client.h"
#include "../include/messages.h"


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
 *  strip_newline macro, strips newline off incoming message.
 */
#define strip_newline(str) str[strlen(str)-1] = '\0'

/**
 *  usage()
 *
 *  \brief  This function prints a help menu for the client.
 */
static void usage(void)
{
    printf("\n");
    printf("List of commands for client\n");
    printf("----------------------------------------------------------\n");
    printf(GREEN " help " RESET "       - Displays help menu\n");
    printf(GREEN " send:" TEAL "<text>" RESET " - Sends " TEAL "<text>" RESET " to the server\n");
    printf("               (note: the ':' must be present)\n");
    printf(GREEN " status " RESET "     - Exits the client and server processes\n");
    printf(GREEN " time " RESET "       - Exits the client and server processes\n");
    printf(GREEN " exit " RESET "       - Exits the client and server processes\n");
    printf("----------------------------------------------------------\n");
    printf("\n");
} /* function usage */




/**
 *
 */
static int request_connection(const char *pipes[])
{
    int srv_fd;
    struct connect_msg cmsg;
    int stat = SUCCESS;

    /* open server FIFO */
    srv_fd = open(SRV_READ, O_WRONLY);

    /* construct connection message */
    cmsg.pid  = getpid();
    strncpy(cmsg.readp,  pipes[READ], CLIENT_RD_NAME_SIZE);
    strncpy(cmsg.writep, pipes[WRITE], CLIENT_WR_NAME_SIZE);

    /* send connection message */
    printf("Attempting to connect to the server...\n");
    if (write(srv_fd, &cmsg, sizeof(struct connect_msg)) < 0)
    {
        fprintf(stderr, "Failed to write connect message for PID: %d with errno: %d\n", getpid(), errno);
        stat = ERROR;
    }

    /* cleanup */
    close(srv_fd);
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
    int exit = FALSE;
    struct pipe_msg in_msg;
    switch (cmd)
    {
        case Send:
            /* strip off 'send:' and '\n' */
            memmove(input, input + 5, MAX_LEN - 5);
            strip_newline(input);
            send_pipe_msg(fd[WRITE], TEXT, NA, input);
            break;

        case Exit:
            printf(RED "Client (PID: %d) exits\n" RESET, getpid());
            send_pipe_msg(fd[WRITE], COMMAND, EXIT, NULL);
            exit = TRUE;
            break;

        case Help:
            usage();
            break;

        case Enter:
            /* do nothing */
            break;

        case Status:
            send_pipe_msg(fd[WRITE], COMMAND, STATUS, NULL);

			/* read the result */
            read(fd[READ], &in_msg, sizeof(struct pipe_msg));
            printf("Number of Clients: %d\n", in_msg.body.status);

            break;

        case Time:
            send_pipe_msg(fd[WRITE], COMMAND, TIME, NULL);

			/* read the result */
            read(fd[READ], &in_msg, sizeof(struct pipe_msg));
            printf("Server Time: %s\n", asctime(&in_msg.body.time));
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
static int run_client(const char *pipes[])
{
    char input[MAX_LEN];
    int fd[2];
    int cmd;
    int exit = FALSE;

    /* open write pipe */
    if ((fd[WRITE] = open(pipes[WRITE], O_WRONLY)) < 0)
    {
        fprintf(stderr, "Error opening %s in func: %s\n", pipes[WRITE], __func__);
        return ERROR;
    }

    /* open read pipe */
    if ((fd[READ] = open(pipes[READ], O_RDONLY)) < 0)
    {
        fprintf(stderr, "Error opening %s in func: %s\n", pipes[READ], __func__);
        return ERROR;
    }

    /* main client loop */
    while (exit != TRUE)
    {
        /* prompt user */
        printf(PINK "client" BLUE "@" GREEN "pid-%d" RESET "$ ", getpid());

        /* get the input */
        memset(input, '\0', sizeof(char)*MAX_LEN);
        fgets(input, MAX_LEN-1, stdin);

        /* handle command */
        cmd  = parse_command(input);
        exit = exec_command(fd, cmd, input);
    }

    /* clean up */
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

    if ((mkfifo(pipes[READ], mode) < 0) && (errno != EEXIST))
    {
        fprintf(stderr, "Error creating %s FIFO\n", pipes[READ]);
        exit(ERROR);
    }

    if ((mkfifo(pipes[WRITE], mode) < 0) && (errno != EEXIST))
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

    if (run_client(pipes) != SUCCESS)
    {
        fprintf(stderr, "Error running client for PID: %d\n", getpid());
        exit(ERROR);
    }


    exit(SUCCESS);
} /* function main */
