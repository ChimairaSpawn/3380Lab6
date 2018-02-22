/*
=================================================================
Lab 6 w/ Sockets
Name: Server.c
Written by : Bryce Hynes - April 2017
Purpose: Create A server, which can send and receive files from a client
Usage ./Server
Parameters: port number ( might be specific )
Subroutines/Libraries included: see # include statements
=================================================================
*/

#include <stdio.h>// standard
#include <stdlib.h>// standard
#include <sys/types.h> // \/ " " \/
#include <sys/socket.h> // used by send() and recv
#include <arpa/inet.h> // in_addr dependancy
#include <netinet/in.h> // network dependancy
#include <signal.h> // signal handling
#include <ctype.h> // it was in the lecture notes ...
#include <fcntl.h> // used byh open()
#include <unistd.h> //POSIX

void catcher(int sig);	

int newfd;

int main(){

	int fd; // internal socket descriptor
	int newfd;
	struct sockaddr_in srv; // bind() for current address info
	struct sockaddr_in cli; // accept() the client for file transmission
	int cli_len = sizeof(cli); // allocate space for the client
	int not_done = 1; //used to measure progress in a boolean
	int readIn, writeOut;
	char buffer[1024];
	char c;
	signal(SIGPIPE, catcher);
	pid_t pid;

	//create socket

	if ( ( fd = socket(AF_INET, SOCK_STREAM, 0) ) < 0 ){
		perror("socket call failed");
		exit(1);
	}
	printf("server: socket initialized\n");
	//bind socket to a port

	srv.sin_family = AF_INET; // internet address family
	srv.sin_port = htons(2055); //bind socket 'fd' to port 202055
	// I did not have access to port 202055.. Its possible someone else has the same last 4 digits, though unlikely.

	//bind so a client can connect to any address
	srv.sin_addr.s_addr = htonl(INADDR_ANY);

	if ( bind ( fd, ( struct sockaddr*)&srv, sizeof(srv) ) < 0 ){
		perror("bind call failed");
		exit(1);
	}
	printf("server: socket bound to port\n");
	
	//listen to the socket for incomming connections
	printf("server: listenning . . .\n");
	if ( listen( fd, 5 ) < 0 ){
		perror("listen call failed");
		exit(1);
	}

	//loop waiting for a message

	while( not_done ){
		//accept any incoming connection
		newfd = accept( fd, ( struct sockaddr* ) &cli, &cli_len );
		if( newfd < 0 ){
			perror("accept call failed");
			exit(1);
			not_done = 0;
		}
		printf("server: connection established with client\n");
		if ( fork() == 0 ){ // fork a child to handle the incomming connection.
			
			recv( newfd, &c, 1024, 0 ); // recieve the incoming message
			printf("server: message recieved from client\n");
			if( ( writeOut = open( &c, O_RDONLY ) ) == -1 ){ 
				printf("SERVER: Could not open file %c \n", c);
				exit(2); 
			}
			printf("server: file openned for transfer\n");

			
			ssize_t nread;
			// send the contents of the sourcefile to the client
			while(nread = read(writeOut, buffer, 1024) > 0){ // check if these is more packets to be sent
				send(newfd, buffer, nread, 0 ); // transmit the file
			}

				
			// close socket when send is done
			close(newfd);
			exit(0);
		}
		else{ // in parent
			close(newfd);
		}
	}

}

void catcher(int sig){
	signal( SIGPIPE, catcher);
	close(newfd);
	exit(0);
}