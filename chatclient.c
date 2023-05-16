#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "structures.h"

void connect_to_server(client_connection *connection, char *address, char *port)
{
	char *addr = malloc(strlen(address)*sizeof(char));
	char *por = malloc(strlen(port)*sizeof(char));
	strcpy(addr,address);
	strcpy(por,port);
	while(true) {

		if ((connection->socket = socket(AF_INET, SOCK_STREAM , IPPROTO_TCP)) < 0) {
				perror("socket creation error");
		}

		connection->address.sin_addr.s_addr = inet_addr(addr);
		connection->address.sin_family = AF_INET;
		connection->address.sin_port = htons(atoi(por));

		if (connect(connection->socket, (struct sockaddr *)&connection->address , sizeof(connection->address)) < 0) {
			perror("connection error.");
			exit(1);
		}
		break;
	}
	puts("connected to server.");
}

void message_handler(client_connection *connection)
{
	message msg;
	ssize_t recv_val = recv(connection->socket, &msg, sizeof(message), 0);
	if(recv_val < 0) {
			perror("reply recieving error");
			exit(1);
	} else { 
        if(recv_val == 0) {
		    close(connection->socket);
		    puts("server disconnected.");
		    exit(0);
	}
	printf("from%s : %s\n", msg.nickname, msg.data);
    }
}

/*void input_handler(client_connection *connection)
{
	char command[MAX_COMMAND_LENGTH];
	char data[MAX_MESSAGE_LENGTH];
	//fgets(input, 255, stdin);


	trim_newline(input);

	if(strncmp(input, "/a", 2) == 0) {			
        message msg;
		strncpy(msg.nickname, connection->nickname, 20);
		if(strlen(input) == 0) {
			return;
		}
		strncpy(msg.data, input, 255);
		if(send(connection->socket, &msg, sizeof(message), 0) < 0) {
				perror("sending error");
				exit(1);
		}
	} else {
		printf("unknown command received\n");
	}
}*/

int main(int argc, char *argv[])
{
	client_connection connection;
	fd_set file_descriptors;
	//printf(argv[1]);
	//printf(argv[2]);
	connect_to_server(&connection, argv[1], argv[2]);
	FD_ZERO(&file_descriptors);
	FD_SET(STDIN_FILENO, &file_descriptors);
	FD_SET(connection.socket, &file_descriptors);
	fflush(stdin);

	if (argc > 4) {
		
		message msg;
		switch(*argv[3]) {
			case GROUP_MESSAGE:
				msg.cmm = GROUP_MESSAGE;
				strncpy(msg.data, argv[4], MAX_MESSAGE_LENGTH);
				strncpy(msg.command_info, argv[5], MAX_MESSAGE_LENGTH);
				if(argc < 6) {
					strncpy(msg.nickname, DEFAULT_NICKNAME, MAX_NICKNAME_LENGTH);
				} else {
					strncpy(msg.nickname, argv[6], MAX_NICKNAME_LENGTH);
				}
				if(send(connection.socket, &msg, sizeof(message), 0) < 0) {
					perror("sending error");
					exit(1);
				}
				break;

			case PRIVATE_MESSAGE:	
				msg.cmm = PRIVATE_MESSAGE;
				strncpy(msg.data, argv[4], MAX_MESSAGE_LENGTH);
				strncpy(msg.command_info, argv[5], MAX_MESSAGE_LENGTH);
				if(argc < 6) {
					strncpy(msg.nickname, DEFAULT_NICKNAME, MAX_NICKNAME_LENGTH);
				} else {
					strncpy(msg.nickname, argv[6], MAX_NICKNAME_LENGTH);
				}
				if(send(connection.socket, &msg, sizeof(message), 0) < 0) {
					perror("sending error");
					exit(1);
				}
				break;

			case EDIT_MESSAGE:
				msg.cmm = EDIT_MESSAGE;
				strncpy(msg.data, argv[4], MAX_MESSAGE_LENGTH);
				strncpy(msg.command_info, argv[5], MAX_MESSAGE_LENGTH);
				if(argc < 6) {
					strncpy(msg.nickname, DEFAULT_NICKNAME, MAX_NICKNAME_LENGTH);
				} else {
					strncpy(msg.nickname, argv[6], MAX_NICKNAME_LENGTH);
				}
				if(send(connection.socket, &msg, sizeof(message), 0) < 0) {
					perror("sending error");
					exit(1);
				}
				break;

			case CREATE_GROUP:
				msg.cmm = CREATE_GROUP;
				strncpy(msg.data, argv[4], MAX_MESSAGE_LENGTH);
				strncpy(msg.command_info, argv[5], MAX_MESSAGE_LENGTH);
				if(argc < 6) {
					strncpy(msg.nickname, DEFAULT_NICKNAME, MAX_NICKNAME_LENGTH);
				} else {
					strncpy(msg.nickname, argv[6], MAX_NICKNAME_LENGTH);
				}
				if(send(connection.socket, &msg, sizeof(message), 0) < 0) {
					perror("sending error");
					exit(1);
				}
				break;

			/*case SEND_FILE_TO_USER:
				message msg;
				msg.cmm = SEND_FILE_TO_USER;
				strncpy(msg.data, argv[4], MAX_MESSAGE_LENGTH);
				FILE *file = fopen(filename, "rb");	
				if(!file) { 
					perror("can't open file"); 
					exit(1);
				}

				if(argc < 6) {
					strncpy(msg.nickname, DEFAULT_NICKNAME, MAX_NICKNAME_LENGTH);
				} else {
					strncpy(msg.nickname, argv[6], MAX_NICKNAME_LENGTH);
				}
				if(send(connection->socket, &msg, sizeof(message), 0) < 0) {
					perror("sending error");
					exit(1);
				}
				break;

			case SEND_FILE_TO_GROUP:
				message msg;
				msg.cmm = GROUP_MESSAGE;
				strncpy(msg.data, argv[4], MAX_MESSAGE_LENGTH);
				if(argc < 5) {
					strncpy(msg.nickname, DEFAULT_NICKNAME, MAX_NICKNAME_LENGTH);
				} else {
					strncpy(msg.nickname, argv[5], MAX_NICKNAME_LENGTH);
				}
				if(send(connection->socket, &msg, sizeof(message), 0) < 0) {
					perror("sending error");
					exit(1);
				}
				break;*/
			default:
				msg.cmm = GROUP_MESSAGE;
				strncpy(msg.data, argv[4], MAX_MESSAGE_LENGTH);
				strncpy(msg.command_info, argv[5], MAX_MESSAGE_LENGTH);
				if(argc < 6) {
					strncpy(msg.nickname, DEFAULT_NICKNAME, MAX_NICKNAME_LENGTH);
				} else {
					strncpy(msg.nickname, argv[6], MAX_NICKNAME_LENGTH);
				}
				if(send(connection.socket, &msg, sizeof(message), 0) < 0) {
					perror("sending error");
					exit(1);
				}
			break;
		}
	} else {
		if (argc != 3) {
			fprintf(stderr,"try: %s <IP> <port>\n", argv[0]);
			exit(1);
		}

		connect_to_server(&connection, argv[1], argv[2]);

		while(true) {
			FD_ZERO(&file_descriptors);
			FD_SET(STDIN_FILENO, &file_descriptors);
			FD_SET(connection.socket, &file_descriptors);
			fflush(stdin);

			if(select(connection.socket+1, &file_descriptors, NULL, NULL, NULL) < 0) {
				perror("selection error.");
				exit(1);
			}
			
			if(FD_ISSET(STDIN_FILENO, &file_descriptors)) {
				//input_handler(&connection);
			}
			
			if(FD_ISSET(connection.socket, &file_descriptors)) {
				
				message_handler(&connection);
			}
		}

		close(connection.socket);
		return 0;
	}
}