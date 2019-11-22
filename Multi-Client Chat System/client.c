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
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>

#define SOCK_PATH "CAOS_socket"

#define BUFFER_SIZE 1024
#define MAX_LINE 10000


void * doRecieving(void * sockID){

	int clientSocket = *((int *) sockID);

	while(1){

		char data[1024];
		int read = recv(clientSocket,data,1024,0);
		data[read] = '\0';
		printf("%s\n",data);

	}

}

void send_recv_message(int i, int s)
// void * send_recv_message(void *input)
{

	char send_buf[BUFFER_SIZE];
	char recv_buf[BUFFER_SIZE];

	if(i == 0)
	{
		fgets(send_buf, BUFFER_SIZE, stdin);
		if(strcmp(send_buf, "exit\n") == 0)
		{
			close(s);
			printf("Client has disconnected from the server.\n");
			exit(0);
		}
		send(s, send_buf, strlen(send_buf), 0);
	}

	else
	{
		int recv_size = recv(s, recv_buf, BUFFER_SIZE, 0);
		recv_buf[recv_size] = '\0';
		printf("%s", recv_buf);

		fflush(stdout);
	}
}

int main(){
	int t, len;
	int clientSocket;
    struct sockaddr_un remote;
    char str[100];
    char* username;
 

    if ((clientSocket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }


    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCK_PATH);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);

    if (connect(clientSocket, (struct sockaddr *)&remote, len) == -1) {
        perror("connect");
        exit(1);
    }

	printf("Connection established ............\n");
	pthread_t thread;
	pthread_create(&thread, NULL, doRecieving, (void *) &clientSocket );
	int done = 0;
	printf("Write GETCLIENTS to get all connected clients \n Write BROADCAST to broadcast message \n Write MSG space Client number space message to send your message\n Write 'exit' to exit\n " );

	while(1){
		int ok=0;
		

		char input[1024];


        if (!input[0], NULL) { 
            
            printf("no input\n");
            continue;
        }
 
		scanf("%s",input);

		while(done==0)
		{
			
			if(strcmp(input,"username") == 0)
			{				
			scanf("%[^\n]s",input);
			send(clientSocket,input,1024,0);
			done=1;
			username= input;
			printf("Welcome %s \n", username );
			}

			else
			{
				printf("Write username space your username\n");
				scanf("%s",input);
				
				// scanf("%s",input);
			}
			ok =1;

		}

		
		if(strcmp(input,"GETCLIENTS") == 0){

			send(clientSocket,input,1024,0);
			ok =1;

		}
		if(strcmp(input,"MSG") == 0){

			send(clientSocket,input,1024,0);
			
			scanf("%s",input);
			send(clientSocket,input,1024,0);
			
			scanf("%[^\n]s",input);
			send(clientSocket,input,1024,0);
			ok =1;

		}

		if (strcmp(input, "exit")==0)
		{
			
			printf("exiting\n");
			send(clientSocket, input, 1024, 0);
			exit(0);
			ok =1;
		}

		if(strcmp(input,"BROADCAST") == 0){

			send(clientSocket,input,1024,0);
			scanf("%[^\n]s",input);
			send(clientSocket,input,1024,0);
			ok =1;

		}
		if (ok==0)
			// (strcmp(input, "BROADCAST")!=0 && strcmp(input, "exit")!=0 && strcmp(input,"MSG") != 0 && strcmp(input,"GETCLIENTS") != 0)
		{
			printf("invalid input\n");
		}

	}
	

	return 0;


}

// struct thread_args {
//     int a;
//     int b;
// };



// struct thread_args *args = malloc(sizeof *args);

// if (args != NULL)
// {
//     
//     pthread_create(&threadId, &attr, &function, args);
// }
