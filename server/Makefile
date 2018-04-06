##
#   \file   Makefile
#   \author James Petersen
#
#   \brief  This is the Makefile that builds the Server
#
#   Usage:
#       make
#       make clean
##

CC     = gcc
CFLAGS = -Wall
OBJ    = main.o server.o
TARGET = cs_2_server

server: $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

main.o: src/main.c include/base.h
	$(CC) $(CFLAGS) -c src/main.c

server.o: src/server.c include/server.h include/base.h
	$(CC) $(CFLAGS) -c src/server.c

clean:
	rm -f $(OBJ)
	rm -f $(TARGET)
