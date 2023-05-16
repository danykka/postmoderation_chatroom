#ifndef _STRUCTURES_H_
#define _STRUCTURES_H_

#define MAX_NICKNAME_LENGTH 32
#define MAX_MESSAGE_LENGTH 256
#define MAX_COMMAND_LENGTH 32

#ifndef DEFAULT_NICKNAME
#define DEFAULT_NICKNAME "ANON"
#endif


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

typedef enum {
	GROUP_MESSAGE,
	PRIVATE_MESSAGE,
	EDIT_MESSAGE,
	CREATE_GROUP,
	SEND_FILE_TO_USER,
	SEND_FILE_TO_GROUP,
	DELETE_USER_MESSAGE,
	DELETE_ALL_USER_MESSAGES,
	DELETE_ALL_GROUP_MESSAGES,
	BAN_USER,
	BAN_GROUP

} command;


typedef struct message {
	char nickname[MAX_NICKNAME_LENGTH];
	char data[MAX_MESSAGE_LENGTH];
	char command_info[MAX_MESSAGE_LENGTH];
	command cmm;
	//FILE *f;
} message;



typedef struct user {
	char nickname[MAX_NICKNAME_LENGTH];
	struct user *next; 

} user;

typedef struct client_connection {
	int socket;
	struct sockaddr_in address;
	char nickname[MAX_NICKNAME_LENGTH];
	struct client_connection *next; 
} client_connection;

client_connection *new_node(int sockfd, struct sockaddr_in addr);

user *new_user(char*usr, char*pwd);

void trim_newline(char *text);

#endif