CC = gcc
DEBUG = -g
CFLAGS = -Wall -pthread -c $(DEBUG)
LFLAGS = -Wall -pthread $(DEBUG)

all:  chatserver chatclient move

chatserver: chatserver.o structures.o
	$(CC) $(LFLAGS) chatserver.o structures.o -o chatserver


chatclient: chatclient.o structures.o
	$(CC) $(LFLAGS) chatclient.o structures.o -o chatclient

move:
	mkdir ./client1
	mkdir ./client2
	cp chatclient ./client1
	cp chatclient ./client2

chatclient.o: chatclient.c structures.h
	$(CC) $(CFLAGS) chatclient.c

chatserver.o: chatserver.c structures.h
	$(CC) $(CFLAGS) chatserver.c

structures.o: structures.h structures.c
	$(CC) $(CFLAGS) structures.c

clean:
	rm -rf *.o *~ chatclient chatserver
	rm -rf client1 client2
