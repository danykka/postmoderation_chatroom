#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "structures.h"

void connect_to_server(client_connection *connection, char *address, char *port) //присоединение серверу
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

void message_handler(client_connection *connection) //получение сообщений от сервера
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

void input_handler(client_connection *connection) //функция-хэндлер для сообщений от клиента
{
	char input[255];
	fgets(input, 255, stdin);
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
}

int main(int argc, char *argv[])
{
	client_connection connection;
	fd_set file_descriptors;

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
			input_handler(&connection);
		}
		
		if(FD_ISSET(connection.socket, &file_descriptors)) {
			
			message_handler(&connection);
		}
	}

	close(connection.socket);
	return 0;
}
