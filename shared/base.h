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

/* Colors */
#define RED   "\e[38;2;255;0;0m"
#define GREEN "\e[38;2;0;255;0m"
#define BLUE  "\e[38;2;0;0;255m"
#define PINK  "\e[38;2;255;0;255m"
#define TEAL  "\e[38;2;0;255;255m"
#define RESET "\e[0m"

/**
 *  Message type
 */
enum message_type
{
    TEXT    = 0,
    COMMAND = 1,
    CONNECT = 2
};


/**
 *  Message struct
 */
struct message
{
    enum message_type type;
    pid_t pid;
    char text[MAX_LEN];
};


#endif
