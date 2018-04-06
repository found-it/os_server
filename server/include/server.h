/**
 *  \file   server.h
 *  \author James Petersen
 *
 *  \brief  
 */

#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int server_handle(const pid_t child_pid);

#endif
