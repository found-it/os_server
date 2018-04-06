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
OBJ    = main.o client.o
TARGET = cs_2_client

client: $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

main.o: src/main.c ../shared/base.h
	$(CC) $(CFLAGS) -c src/main.c

client.o: src/client.c include/client.h ../shared/base.h
	$(CC) $(CFLAGS) -c src/client.c

clean:
	rm -f $(OBJ)
	rm -f $(TARGET)
