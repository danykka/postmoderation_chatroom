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

chatclient.o: ./src/chatclient.c structures
	$(CC) $(CFLAGS) chatclient.c
	mv chatclient.o ./obj

chatserver.o: ./src/chatserver.c ./lib/structures.h
	$(CC) $(CFLAGS) ./src/chatserver.c
	mv chatserver.o ./obj

structures.o: ./lib/structures.h ./src/structures.c
	$(CC) $(CFLAGS) ./src/structures.c
	mv structures.o ./obj

clean:
	rm -rf *.o *~ chatclient chatserver
	rm -rf client1 client2
