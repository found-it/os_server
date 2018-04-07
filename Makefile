
##
#   \file   Makefile
#   \author James Petersen
#
#   \brief  This is the Makefile that builds the Client
#
#   Usage:
#       make
#       make clean
##

CC     = gcc
CFLAGS = -Wall
OBJ_C  = client.o
TAR_C  = cs_2_client
OBJ_S  = server.o
TAR_S  = cs_2_server
PIPES  = ./pipes

all: server client

server: $(OBJ_S)
	$(CC) $(CFLAGS) -o $(TAR_S) $(OBJ_S)

server.o: src/server.c include/server.h include/base.h
	$(CC) $(CFLAGS) -c src/server.c

client: $(OBJ_C)
	$(CC) $(CFLAGS) -o $(TAR_C) $(OBJ_C)

client.o: src/client.c include/client.h include/base.h
	$(CC) $(CFLAGS) -c src/client.c

clean:
	rm -f $(OBJ_C) $(OBJ_S)
	rm -f $(TAR_C) $(TAR_S)
	rm -f $(PIPES)/*_read $(PIPES)/*_write
