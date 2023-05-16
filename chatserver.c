#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include "structures.h"

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

int server_sockfd = 0, client_sockfd = 0, clientcount = 0;

FILE * serverlog;

client_connection *server_info;
client_connection *root,*now;
user *usroot;

void server_init(int port)
{
	serverlog = fopen("server.log","w");

	server_info = (client_connection *)malloc(sizeof(client_connection));
	if((server_info->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket creation error");
		exit(1);
	}

	server_info->address.sin_family = AF_INET;
	server_info->address.sin_addr.s_addr = INADDR_ANY;
	server_info->address.sin_port = htons(port);

	if(bind(server_info->socket, (struct sockaddr *)&server_info->address, sizeof(server_info->address)) < 0) {
		perror("binding error");
		exit(1);
	}

	const int optVal = 1;
	const socklen_t optLen = sizeof(optVal);
	if(setsockopt(server_info->socket, SOL_SOCKET, SO_REUSEADDR, (void*) &optVal, optLen) < 0) {
		perror("socket option setting error");
		exit(1);
	}

	if(listen(server_info->socket, 5) < 0) {
		perror("listen error");
		exit(1);
	}

	printf("starting server on: %s:%d\n", inet_ntoa(server_info->address.sin_addr), ntohs(server_info->address.sin_port));
	printf("waiting for users...\n");
	fprintf(serverlog,"starting server on: %s:%d\n", inet_ntoa(server_info->address.sin_addr), ntohs(server_info->address.sin_port));
	fprintf(serverlog,"waiting for users...\n");

}

void delete_connection(client_connection *p_client)
{
	client_connection *cur = root,*prev;
	while(cur != NULL) {
		if(cur->socket == p_client->socket) {
			if(cur == root) {
				root = cur->next;
			}
			else {
				prev->next = cur->next;
			}
			free(cur);
			clientcount--;
			return;
		}
		prev = cur;
		cur = cur->next;
	}
	printf("critical error\n");
	return;
}

void send_message(client_connection *p_client, char *message_text) 
{
	message msg;
	strncpy(msg.nickname, p_client->nickname, MAX_NICKNAME_LENGTH);
	strncpy(msg.data, message_text, MAX_MESSAGE_LENGTH);
	printf("public message sent by %s:%s\n", p_client->nickname,  message_text);
	pthread_mutex_lock( &mutex2 );
	fprintf(serverlog, "public message sent by %s:%s\n", p_client->nickname,  message_text);
	pthread_mutex_unlock( &mutex2 );
	client_connection *curr = root;
	while(curr != NULL) {
		if(send(curr->socket, &msg, sizeof(msg), 0) < 0) {
			perror("message sending error");
			exit(1);
		}
		curr = curr->next;
	}

	return;
}

void handle_client_message(void *p_client) 
{
	int read_size;
	int leave_flag = 0;
	client_connection *pt = (client_connection *)p_client;
	message msg;
	//printf("handle\n");
	while(1) {
		if(leave_flag) {
			break;
		}
		if((read_size = recv(pt->socket, &msg, sizeof(message), 0)) == 0) {
			printf("client %s:%d disconnected.\n", inet_ntoa(pt->address.sin_addr), ntohs(pt->address.sin_port));
			pthread_mutex_lock( &mutex2 );
			fprintf(serverlog, "client %s:%d disconnected.\n", inet_ntoa(pt->address.sin_addr), ntohs(pt->address.sin_port));
			pthread_mutex_unlock( &mutex2 );
			leave_flag = 1;
		} else {
			send_message(pt, msg.data);
		}
	}
	pthread_mutex_lock( &mutex1 );
	delete_connection(pt);
	pthread_mutex_unlock( &mutex1 );
	close(pt->socket);
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) 
{
	
	puts("starting server...");
	
	int server_len,client_len;
	struct sockaddr_in client;

	if(argc != 2) {
		fprintf(stderr, "try: %s <port>\n", argv[0]);
		exit(1);
	}

	server_init(atoi(argv[1]));

	while(true) {
		client_sockfd = accept(server_info->socket, (struct sockaddr*)&server_info->address, (socklen_t*)&server_len);

		if(client_sockfd < 0) {
			perror("acception error");
			exit(1);
		}

		getpeername(client_sockfd, (struct sockaddr*) &client, (socklen_t*) &client_len);
		printf("client %s:%d joined.\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		pthread_mutex_lock( &mutex2 );
		fprintf(serverlog, "client %s:%d joined.\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		pthread_mutex_unlock( &mutex2 );
		
		if(clientcount ==0) {
			root = new_node(client_sockfd,client);
			now = root;
			clientcount++;
		} else {
			client_connection *c;
			c = new_node(client_sockfd,client);
			now = root;
			while(now!=NULL) {
				if(now->next == NULL) {
					now->next = c;
					now = c;
					break;
				}
				now = now->next;
			}			
			clientcount++;
		}
		
		pthread_t id;
		if(pthread_create(&id, NULL, (void *)handle_client_message, (void *)now) != 0) {
			perror("pthread creating error!\n");
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}
