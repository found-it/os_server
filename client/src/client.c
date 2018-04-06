/**
 *  \file   client.c
 *  \author James Petersen
 *
 *  \brief  This is the main file running the client.
 */

#include "../include/client.h"
#include "../../shared/base.h"

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
    printf(GREEN " exit " RESET "       - Exits the client and server processes\n");
    printf("----------------------------------------------------------\n");
    printf("\n");
} /* function usage */



/**
 *  client_handle()
 *
 *  \brief  This is the client function that writes
 *          to the pipe.
 */
int client_handle(const int fd[])
{
    struct message msg;
    char input[MAX_LEN];

    /* close the reading side of the pipe */
    close(fd[READ]);

    /* main client loop */
    while (1)
    {
        /* prompt user */
        printf(PINK "client" BLUE "@" GREEN "pid-%d" RESET "$ ", getpid());

        /* get the input */
        memset(input, '\0', sizeof(char)*MAX_LEN);
        fgets(input, MAX_LEN, stdin);

        /* parse command */
        if (strncmp(input, "send:", 5) == 0)        /* send command */
        {
            /* strip off 'send:' */
            memmove(input, input + 5, MAX_LEN - 5);

            msg.type = TEXT;
            strncpy(msg.text, input, MAX_LEN);
            write(fd[WRITE], &msg, sizeof(struct message));
        }
        else if (strncmp(input, "exit", 4) == 0)    /* exit command */
        {
            printf(RED "Client (PID: %d) exits\n" RESET, getpid());
            msg.type = COMMAND;
            write(fd[WRITE], &msg, sizeof(struct message));
            break;
        }
        else if (strncmp(input, "help", 4) == 0)    /* help command */
        {
            usage();
        }
        else if (strncmp(input, "\n", 1) == 0)      /* ignore stray <return> */
        {
            continue;
        }
        else                                        /* incorrect command */
        {
            printf("Incorrect command. Type 'help' for list of commands.\n");
        }
    }
    _exit(SUCCESS);
} /* function client_handle */


