/*
=================================================================
Lab 6 w/ Sockets
Name: Client.c
Written by : Bryce Hynes - April 2017
Purpose: Create a client to connect to Server.c 
Usage ./Client ""
Parameters: port number ( might be specific )
Subroutines/Libraries included: see # include statements
=================================================================
*/

#include <stdio.h>// standard
#include <stdlib.h>// standard
#include <arpa/inet.h> //sockaddr_in && inet_addr()
#include <sys/socket.h> // used by send() recv()
#include <sys/types.h> // " "
#include <netinet/in.h> // network dependancies
#include <fcntl.h> //used by open()
#include <unistd.h> //POSIX
int main(int argc, char *argv[]){

	if(argc != 3){ 
		printf("Incorrect arguments.  Please structure as: ./Client /path/to/file/on/server /path/to/file/on/client \n");
		exit(0);
	}

	int fd;  // socket descriptor
	struct sockaddr_in srv; //used by connect ()
	char c, rc;
	char *file2retrieve = argv[1]; // "/home/common/lab_sourcefile"
	char *file2write = argv[2]; // "/home/brycehynes/Documents/lab6/lab_sourcefile_local_clone"
	int more_data = 1; // boolean for more data to transfer
	char buffer [ 1024 ]; // buffer size, for send/recv
	ssize_t nread;
	long total;



	//socket
	if( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) <0 ){
		perror("socket call failed");
		exit(1);
	}
	printf("client: socket initialized");

	//connect 
	srv.sin_family = AF_INET;
	srv.sin_port = htons(2055); // connect to port 202055
	srv.sin_addr.s_addr = inet_addr("192.197.151.70"); // ip for odin / frigg

	if( connect( fd, ( struct sockaddr* ) &srv, sizeof( srv ) ) < 0 ){
		perror("conect call failed");
		exit(1);
	}
	printf("client: connection established with host");

	//open file to write to
	int readIn, writeOut;
	if( writeOut = ( open(file2write, O_WRONLY | O_CREAT | O_TRUNC, 0644 ) < 0 ) ){
		printf("Could not open file %s\n", argv[1] );
		exit(2);
	}
	printf("client: file created for transfer");

	//send and recieve information 
	printf("client: beginning file transfer");
	while( more_data ){
			
			send( fd, &file2retrieve, 1024, 0); 
			
			while( recv(	fd, &file2write, 1024, 0) > 0 ){ 
				write(writeOut, &rc, nread);
				total =+ nread;
				//printf("%c\n", rc); // don't need to print anything as it's received
			}
			more_data = 0;			
	}
				printf("client: tranfer complete \n %ld bytes transferred\n", total);
	exit(0);
} 	
