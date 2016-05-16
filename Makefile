CC = gcc
CFLAGS = -Wall -g -Wextra -std=c99

all: SimpServer SimpClient

clean:
	rm -f SimpServer SimpClient *.o

SimpServer : SimpServer.o
	$(CC) $(CFLAGS) -o $@ $^ -lnsl

SimpClient : SimpClient.o
	$(CC) $(CFLAGS) -o $@ $^ -lnsl

.PHONY : all clean
