#ifndef _STRUCTURES_H_
#define _STRUCTURES_H_

#define MAX_NICKNAME_LENGTH 32
#define MAX_PASSWORD_LENGTH 32
#define MAX_MESSAGE_LENGTH 256

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

//message structure
typedef struct message {
	char nickname[MAX_NICKNAME_LENGTH];
	char data[MAX_MESSAGE_LENGTH];
} message;

typedef struct user {
	char nickname[MAX_NICKNAME_LENGTH];
	char password[MAX_PASSWORD_LENGTH];
	struct user *next; 
} user;

typedef struct client_connection {
	int socket;
	struct sockaddr_in address;
	char nickname[MAX_NICKNAME_LENGTH];
	struct client_connection *next; 
} client_connection;

client_connection *newNode(int sockfd, struct sockaddr_in addr);

user *newUser(char*usr,char*pwd);

void trim_newline(char *text);

#endif