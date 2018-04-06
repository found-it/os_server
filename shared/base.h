/**
 *  \file   base.h
 *  \author James Petersen
 *
 *  \brief  
 */

#ifndef BASE_H
#define BASE_H


/* Global defines */
#define SUCCESS  0
#define ERROR   -1
#define MAX_LEN  100
#define WRITE    1
#define READ     0

#define CLIENT_WR_NAME_SIZE 30
#define CLIENT_RD_NAME_SIZE 27

/* Colors */
#define RED   "\e[38;2;255;0;0m"
#define GREEN "\e[38;2;0;255;0m"
#define BLUE  "\e[38;2;0;0;255m"
#define PINK  "\e[38;2;255;0;255m"
#define TEAL  "\e[38;2;0;255;255m"
#define RESET "\e[0m"

#define SRV_READ  "/tmp/server_np"

/**
 *  Message type
 */
enum msg_type
{
    TEXT    = 0,
    COMMAND = 1
};


/**
 *  Message struct
 */
struct pipe_msg
{
    enum msg_type type;
    char text[MAX_LEN];
};

struct connect_msg
{
    pid_t pid;
    char readp[CLIENT_RD_NAME_SIZE];
    char writep[CLIENT_WR_NAME_SIZE];
};

#endif
