#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <unistd.h>   
#include <arpa/inet.h>    
#include <sys/types.h>
#include <errno.h>

#define MAX_CLIENTS 2
#define BUFFER_SIZE 32
#define SHUTDOWN_LENGTH 30

void print_help() {
  fprintf(stderr, "Usage: ./server <port>\n");
}

int sockets_full(int sockets[]) {
	int i;
	int result = 1;
	for (i=0; i<MAX_CLIENTS; i++) {
		if (sockets[i]==0) {
	 	 	result = 0;
		}
	}
	return result;
}

int main(int argc, char *argv[]) {
  int i, sock_fd, port, master_socket, client_socket[MAX_CLIENTS];
  int activity, valread, new_socket, sd, max_sd, test_int;
  int err=0;
  int shutdown=0;
  bool done = false;
  struct sockaddr_in self;
  struct timeval tv;
  fd_set readfds;
  char *welcome_message = "Connection Successfull \n";
  char *message2 = "ERROR No free socket \n";
  char *shutdown_message = "Server was shut down\n";
  char *ok_shutdown = "OK SHUTDOWN\n";
  char *error_message_too_long = "ERROR Message too long\n";
  char *error_message_unknown_command = "ERROR Unknown Command \n";
  char *error_empty_string = "ERROR string empty \n";
  char *error_wrong_pass = "ERROR wrong password \n";
  char *ok_bye = "OK BYE\n";
  char buffer[BUFFER_SIZE];  
  char * first_word;
  
  bzero(buffer, BUFFER_SIZE);
  if (argc != 2) {
    fprintf(stderr, "Invalid number of arguments!\n");
    print_help();
    return 1;
  }
  // parse port
  if (sscanf(argv[1], "%d", &port) != 1) {
    fprintf(stderr, "Can't parse port: '%s'\n", argv[2]);
    print_help();
    return 1;
  }
  //init client_sockets
  for (i=0; i<MAX_CLIENTS; i++) {
  	client_socket[i] = 0; 
  }
  //create master socket
  if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) {
  	perror("socket failed");
  	exit(EXIT_FAILURE);
  }
  // prepare bind
  bzero(&self, sizeof(self));
  self.sin_family = AF_INET;
  self.sin_port   = htons(port);
  self.sin_addr.s_addr = INADDR_ANY;
  
  //bind the master socket to port
  if (bind(master_socket, (struct sockaddr *)&self, sizeof(self))<0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  printf("Listener on port %d \n", port);
  //try to specify maximum of 3 pending connections for the master socket
  if (listen(master_socket, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  //accept the incoming connection
  int addrlen = sizeof(self);
  puts("Waiting for connections ...");
    
  while (!done) {
  	//clear the socket set
    FD_ZERO(&readfds);
  
    //add master socket to set
    FD_SET(master_socket, &readfds);
    max_sd = master_socket;
    //add child sockets to set
    for ( i = 0 ; i < MAX_CLIENTS ; i++) {
    	//socket descriptor
      sd = client_socket[i];
             
      //if valid socket descriptor then add to read list
      if(sd > 0) {
        FD_SET( sd , &readfds);
      }       
      //highest file descriptor number, need it for the select function
      if(sd > max_sd) {
        max_sd = sd;
      }
    }
    // init timeval for shutdown
    tv.tv_sec = SHUTDOWN_LENGTH;
    tv.tv_usec = 0;
    
    //wait for an activity on one of the sockets
    if (shutdown) {
    	printf("Shutdown in %d seconds\n",SHUTDOWN_LENGTH);
    	activity = select( max_sd + 1 , &readfds , NULL , NULL , &tv);
    } else {
    	activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
    }
    
    if ((activity < 0) && (errno!=EINTR)) {
      printf("select error");
    } else if (!activity) {
    	// when in shutdown mode and no activity occured within SHUTDOWN_LENGTH seconds 
    	// close all sockets and exit server
      for (i = 0; i < MAX_CLIENTS; i++) {
      	sd = client_socket[i];
      	if (sd) {
      		close(sd);
       		printf("Socket %d was closed\n",i);
      	}
      }
    	exit(0);
    }
    //If something happened on the master socket , then its an incoming connection
    if (FD_ISSET(master_socket, &readfds)) {
      if ((new_socket = accept(master_socket, (struct sockaddr *)&self,
    			(socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
      }
          
      //inform user of socket number - used in send and receive commands
      printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , 
      new_socket , inet_ntoa(self.sin_addr) , ntohs(self.sin_port));
          
      if ((!sockets_full(client_socket))&&(!shutdown)) {
      	//send new connection greeting welcome_message
      	if( send(new_socket, welcome_message, strlen(welcome_message), MSG_NOSIGNAL) != strlen(welcome_message) ) {
         	perror("send");
       	}
       	puts("Welcome welcome_message sent successfully");
       	//add new socket to array of sockets
       	for (i = 0; i < MAX_CLIENTS; i++) {
         	//if position is empty
         	if( client_socket[i] == 0 ) {
           	client_socket[i] = new_socket;
           	printf("Adding to list of sockets as %d\n" , i); 
         		break;
         	}
      	}
     	} else {
     		// send connection rejection message and close socket
     	 	if( send(new_socket, message2, strlen(message2),MSG_NOSIGNAL) != strlen(message2) ) {
       		perror("send");
				}
	      printf("connection rejected, maximum number %d of clients reached \n", MAX_CLIENTS);
       	close(new_socket);
     	}
   	}   
   	//else its some IO operation on some other socket
   	for (i = 0; i < MAX_CLIENTS; i++) {
    	sd = client_socket[i];    
    	if (FD_ISSET( sd , &readfds)) {
    	  //Check if it was for closing , and also read the incoming welcome_message
   		  if ((valread = read( sd , buffer, 1024)) == 0) {
      		//Somebody disconnected , get his details and print
        	getpeername(sd , (struct sockaddr*)&self , (socklen_t*)&addrlen);
        	printf("Host disconnected , ip %s , port %d \n" , inet_ntoa
        	(self.sin_addr) , ntohs(self.sin_port));
                   
        	//Close the socket and mark as 0 in list for reuse
        	close( sd );
        	client_socket[i] = 0;
      	} else {
      		//Check size of received message
      		if (valread>BUFFER_SIZE) {
      			err=1;
      			send(sd, error_message_too_long, strlen(error_message_too_long),MSG_NOSIGNAL);
      		}
      		if (!err) {
        		first_word = strtok(buffer," ");
      	  	// check for keywords
      	  	if (!strcmp(first_word,"SHUTDOWN")) {
      	  		printf("Shutdown angekommen\n");
      	  		send(sd,ok_shutdown,strlen(ok_shutdown),MSG_NOSIGNAL);
      	  		shutdown=1;
      	  	} else if (!strcmp(first_word,"ECHO")) {
      	  		buffer[valread] = '\0';
        			send(sd , buffer , strlen(buffer) ,  MSG_NOSIGNAL );
        		} else if (!strcmp(buffer,"BYE")) {
        			send(sd,ok_bye,strlen(ok_bye),MSG_NOSIGNAL);
        			printf("client %d on socket %d disconnected\n", i, sd);
        			close(sd);
        			client_socket[i]=0;
      	  	} else {
      				send(sd, error_message_unknown_command, strlen	
      				(error_message_unknown_command),MSG_NOSIGNAL);
      	  	}
        	}
        	err=0;
      	}
    	}
  	}
 	}         
 	return 0;
}
