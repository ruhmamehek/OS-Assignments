#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
// #include <fcntl.h>
#include <pthread.h>
// #include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
// #include <arpa/inet.h>

#define SOCK_PATH "CAOS_socket"
#define BUFFER_SIZE 1024



char usernames[1024][256];
int clientCount = 0;


static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


struct client{

	int index;
	int exist;
	int sockID;
	struct sockaddr_un clientAddr;
	int len;
	char *username;
	

};
struct client Client[1024];
pthread_t thread[1024];

void * talkToClient(void * ClientDetail){

	struct client* clientDetail = (struct client*) ClientDetail;
	int index = clientDetail -> index;
	int clientSocket = clientDetail -> sockID;

	printf("Client %d connected.\n",index + 1);

	while(1){

		char data[1024];
		int read = recv(clientSocket,data,1024,0);
		data[read] = '\0';

		char output[1024];

		if(strcmp(data,"GETCLIENTS") == 0){

			int l = 0;

			for(int i = 0 ; i < clientCount ; i ++){

				if(i != index)
					if (Client[i].exist==1)
					l += snprintf(output + l,1024,"Client %d is at socket %d, username: %s. \n",i + 1,Client[i].sockID, Client[i].username);

			}

			if ((send(clientSocket,output,1024,0), 0)<0){perror("Error in sending data");}
			continue;

		}
		if (strcmp(data, "exit")==0)
		{
			Client[index].exist=0;
			int l = 0;
			l += snprintf(output + l,1024,"Client %d has exited ", index+1);
			// for(int i = 0 ; i < clientCount ; i ++){

			// 	if(i != index)
			// 	{
			// 		if ((send(Client[i].sockID, output,1024,0), 0) < 0)
			// 			perror("Error in sending data");
			// 	}

			// }
			close(clientSocket);
		}

		if(strcmp(data,"BROADCAST") == 0){
					read = recv(clientSocket,data,1024,0);
					data[read] = '\0';


		for(int i = 0 ; i < clientCount ; i ++){


				if(i != index && Client[i].exist==1)
				{
					int l = 0;
					l += snprintf(output + l,1024,"%s(Client %d) says: ", Client[index].username, index+1);
					
					if ((send(Client[i].sockID,output,1024,0),0)<0){perror("Error in sending data");}
					if ((send(Client[i].sockID,data,1024,0),0)<0)perror("Error in sending data");
				
				}

			}

		}

		if(strcmp(data,"MSG") == 0){

			read = recv(clientSocket,data,1024,0);
			data[read] = '\0';

			int id = atoi(data) - 1;

			if (id==0)
			{
				if ((send(clientSocket,"invalid client",1024,0), 0)<0){perror("Error in sending data");}
			}
			else
			{ 
							read = recv(clientSocket,data,1024,0);
			data[read] = '\0';
			if (Client[id].exist==1){		
			int k = 0;
			k += snprintf(output + k,1024,"%s(Client %d) says: ", Client[index].username, index+1);
			if ((send(Client[id].sockID,output,1024,0), 0)<0)perror("Error in sending data");
			if ((send(Client[id].sockID,data,1024,0),0)<0){perror("Error in sending data");}
			}
			else
			{
				if ((send(clientSocket,"Client doesn't exist",1024,0), 0)<0){perror("Error in sending data");}
			}	
			}

		
		}

	}

	return NULL;

}

void socketSetup(int *s)
{
	*s = socket(AF_UNIX, SOCK_STREAM, 0);
	if ( *s == -1) {
		perror("socket");
		exit(1);
	}
}


void socketBinding(int s, struct sockaddr_un local)
{
	int len;
	int retval;

	int reuse = 1;
	if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(int)) < 0)
	{
		perror("Unable to set reuse option on socket");
		exit(1);
	}

	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, SOCK_PATH);
	unlink(local.sun_path);
	len = strlen(local.sun_path) + sizeof(local.sun_family);
	retval = bind(s, (struct sockaddr *)&local, len);
	if ( retval == -1) {
		perror("bind");
		exit(1);
	}

}

void listenSocket(int s)
{
	if (listen(s, 5) == -1) 
	{
		perror("listen");
		exit(1);
		
	}
	printf("Server connected, listening");

	fflush(stdout);
}

void checkConnection(int client_sockfd, int s, struct sockaddr_un *remote)
{

		if( client_sockfd < 0)
		{
			perror("Unable to accept connection");
			exit(1);
		}

		Client[clientCount].sockID = client_sockfd;
		Client[clientCount].index = clientCount;
		Client[clientCount].exist=1;

		printf("Client at socket %d has been connected \n", client_sockfd);
			char *username_prompt = "Enter your username: ";
		if(send(client_sockfd, username_prompt, strlen(username_prompt), 0) < 0)
		perror("Unable to prompt username");

		int recv_size;

		int done = 0;
		do
		{
			if((recv_size = recv(client_sockfd, usernames[client_sockfd], 256, 0)) <= 1)
			{
				if(recv_size == 1)
				{
					char *retry_message = "The username cannot be empty, try again: ";
					if(send(client_sockfd, retry_message, strlen(retry_message), 0) < 0)
						perror("Error sending retry message");
				}

				else
				{
					if(recv_size == 0)
						printf("Connection at socket %d has been terminated\n", client_sockfd);
					else
						perror("Error in receiving message from client");
					close(client_sockfd);
					// FD_CLR(client_sockfd, curr_fds);
				}

			}
			else
				done = 1;
		} while(!done);

		usernames[client_sockfd][recv_size - 1] = '\0';
		printf("Client at socket %d has been assigned username '%s'\n", client_sockfd, usernames[client_sockfd]);
		Client[clientCount].username = usernames[client_sockfd];

		pthread_create(&thread[clientCount], NULL, talkToClient, (void *) &Client[clientCount]);

		clientCount ++;

}

int main(void)
{

	int s, s2;
	unsigned t;
	struct sockaddr_un local, remote;
	char str[100];
	

	socketSetup(&s);
	socketBinding(s, local);
	listenSocket(s);
	int client_sockfd;

	while(1)
	{
		client_sockfd = accept(s, (struct sockaddr*) &Client[clientCount].clientAddr, &Client[clientCount].len);
		checkConnection(client_sockfd, s, &remote);

	}


	for(int i = 0 ; i < clientCount ; i ++)
		pthread_join(thread[i],NULL);










	return 0;
}