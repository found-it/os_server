/**
 *  \file   client.c
 *  \author James Petersen
 *
 *  \brief  This is the main file running the client.
 */


#include "../include/base.h"
#include "../include/client.h"
#include "../include/messages.h"


/**
 *  All the client commands that are available.
 */
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
 *  strip_newline macro, strips newline off outgoing message.
 */
#define strip_newline(str) str[strlen(str)-1] = '\0'


/**
 *  usage()
 *
 *  \brief  Prints a help menu for the client.
 */
static void usage(void)
{
    printf("\n");
    printf("List of commands for client\n");
    printf("----------------------------------------------------------\n");
    printf(GREEN " help " RESET "       - Displays help menu\n");
    printf(GREEN " send:" TEAL "<text>" RESET " - Sends " TEAL "<text>" RESET " to the server\n");
    printf("               (note: the ':' must be present)\n");
    printf(GREEN " time " RESET "       - Returns the current date and time from the server.\n");
    printf(GREEN " status " RESET "     - Returns the number of clients connected to the server.\n");
    printf(GREEN " exit " RESET "       - Exits the client and server processes\n");
    printf("----------------------------------------------------------\n");
    printf("\n");
}


/**
 *  request_connection()
 *
 *  \brief  Attempts to connect to the server.
 */
static int request_connection(void)
{
    int srv_fd;
    struct connect_msg cmsg;
    int stat = SUCCESS;

    /* open server FIFO */
    srv_fd = open(SRV_FIFO, O_WRONLY);

    /* construct connection message */
    cmsg.pid  = getpid();

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
 *  parse_command()
 *
 *  \brief  Parses the input string to get the command.
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

    else if (strncmp(input, "status", 6) == 0)  /* status command */
        return Status;

    else if (strncmp(input, "time", 4) == 0)    /* time command */
        return Time;

    else                                        /* incorrect command */
        return ERROR;
}


/**
 *  client_send()
 *
 *  \brief  Sends the input string to the server.
 */
static inline void client_send(const int fd, char *input)
{
    /* strip off 'send:' and '\n' */
    memmove(input, input + 5, MAX_LEN - 5);
    strip_newline(input);
    send_pipe_msg(fd, TEXT, NA, input);
}


/**
 *  client_exit()
 *
 *  \brief  Sends exit command to the server.
 */
static inline void client_exit(const int fd)
{
    printf(RED "Client (PID: %d) exits\n" RESET, getpid());
    send_pipe_msg(fd, COMMAND, EXIT, NULL);
}


/**
 *  get_status()
 *
 *  \brief  Gets the number of connected clients from the server.
 */
static inline void get_status(const int fd[])
{
    struct pipe_msg in_msg;
    send_pipe_msg(fd[WRITE], COMMAND, STATUS, NULL);

    /* read the result */
    read(fd[READ], &in_msg, sizeof(struct pipe_msg));
    printf("Number of Clients: %d\n\n", in_msg.body.status);
}


/**
 *  get_time()
 *
 *  \brief  Gets the time from the server.
 */
static inline void get_time(const int fd[])
{
    struct pipe_msg in_msg;
    send_pipe_msg(fd[WRITE], COMMAND, TIME, NULL);

    /* read the result */
    read(fd[READ], &in_msg, sizeof(struct pipe_msg));
    printf("Server Time: %s\n", asctime(&in_msg.body.time));
}


/**
 *  exec_command()
 *
 *  \brief  Executes the commands issued to the client by the user.
 */
static int exec_command(const int fd[], const int cmd, char *input)
{
    int exit = FALSE;
    switch (cmd)
    {
        case Send:
            client_send(fd[WRITE], input);
            break;

        case Exit:
            client_exit(fd[WRITE]);
            exit = TRUE;
            break;

        case Help:
            usage();
            break;

        case Enter:
            /* do nothing */
            break;

        case Status:
            get_status(fd);
            break;

        case Time:
            get_time(fd);
            break;

        default:
            printf("Incorrect command. Type 'help' for list of commands.\n");
            break;
    }
    return exit;
}


/**
 *  run_client()
 *
 *  \brief  This is the main client operation loop.
 */
static int run_client(const char *pipes[])
{
    char input[MAX_LEN];
    int fd[2];
    int cmd;
    int exit = FALSE;

    /* display the usage */
    usage();

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
 *  \brief  The main client loop function.
 */
int main(void)
{
    char read_pipe[CLIENT_PIPE_NAME_SIZE];
    char write_pipe[CLIENT_PIPE_NAME_SIZE];
    const char *pipes[2];
    mode_t mode;

    /* create the FIFO name strings */
    snprintf(read_pipe,  CLIENT_PIPE_NAME_SIZE, "./pipes/%d_read",  getpid());
    snprintf(write_pipe, CLIENT_PIPE_NAME_SIZE, "./pipes/%d_write", getpid());

    /* put the FIFO names into an array */
    pipes[READ]  = read_pipe;
    pipes[WRITE] = write_pipe;

    /* set the permissions of the FIFO's */
    mode = S_IRUSR | S_IWUSR;

    /* create the clients read FIFO */
    if ((mkfifo(pipes[READ], mode) < 0) && (errno != EEXIST))
    {
        fprintf(stderr, "Error creating %s FIFO\n", pipes[READ]);
        exit(ERROR);
    }

    /* create the clients write FIFO */
    if ((mkfifo(pipes[WRITE], mode) < 0) && (errno != EEXIST))
    {
        fprintf(stderr, "Error creating %s FIFO\n", write_pipe);
        exit(ERROR);
    }

    /* attempt to connect to the server */
    if (request_connection() != SUCCESS)
    {
        fprintf(stderr, "Connection request failed for PID: %d\n", getpid());
        exit(ERROR);
    }
    else
        printf("Connected to the server.\n");

    /* run the main client loop */
    if (run_client(pipes) != SUCCESS)
    {
        fprintf(stderr, "Error running client for PID: %d\n", getpid());
        exit(ERROR);
    }

    exit(SUCCESS);
}
