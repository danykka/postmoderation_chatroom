#include "structures.h"

#include <string.h>
#include <stdlib.h>

client_connection *newNode(int sockfd, struct sockaddr_in addr) {
	client_connection *np = (client_connection *)malloc( sizeof(client_connection) );
	np->socket = sockfd;
	np->next = NULL;
	np->address = addr;

	return np;
}

// new a user datatype
user *newUser(char*usr,char*pwd){
	user *nu = (user *)malloc( sizeof(user) );
	strcpy(nu->nickname,usr);
	strcpy(nu->password,pwd);
	nu->next = NULL;
	return nu;
}

void trim_newline(char *text)
{
  int len = strlen(text) - 1;
  if (text[len] == '\n')
{
      text[len] = '\0';
  }
}