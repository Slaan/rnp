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

#define MAX_CLIENTS 3
#define BUFFER_SIZE 255

/*void* connection_handler(int sock) {
  int err;
  char buffer[BUFFER_SIZE];  
  bzero(buffer, BUFFER_SIZE);
  err = read(sock, buffer, BUFFER_SIZE);
  if (err == 0) {
    fprintf(stderr, "Couldn't read! %d", sock);
    // exit thread
  }
}*/

void print_help() {
  fprintf(stderr, "Usage: ./server <port>\n");
}

int main(int argc, char *argv[]) {
  int i, err, sock_fd, port, master_socket, client_socket[MAX_CLIENTS];
  int activity, valread, new_socket, sd, max_sd;
  bool done = false;
  struct sockaddr_in self;
  fd_set readfds;
  char *message = "Connection Successfull \r\n";
  char buffer[BUFFER_SIZE];  
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
  if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
  
  
  
  // prepare bind
  bzero(&self, sizeof(self));
  self.sin_family = AF_INET;
  self.sin_port   = htons(port);
  self.sin_addr.s_addr = INADDR_ANY;
  
  //bind the master socket to port
    if (bind(master_socket, (struct sockaddr *)&self, sizeof(self))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", port);
    
    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
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
        for ( i = 0 ; i < MAX_CLIENTS ; i++)
        {
            //socket descriptor
            sd = client_socket[i];
             
            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET( sd , &readfds);
             
            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
                max_sd = sd;
        }
  
        //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
    
        if ((activity < 0) && (errno!=EINTR))
        {
            printf("select error");
        }
          
        //If something happened on the master socket , then its an incoming connection
        if (FD_ISSET(master_socket, &readfds))
        {
            if ((new_socket = accept(master_socket, (struct sockaddr *)&self,
            	(socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
          
            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , 
            	 new_socket , inet_ntoa(self.sin_addr) , ntohs(self.sin_port));
        
            //send new connection greeting message
            if( send(new_socket, message, strlen(message), 0) != strlen(message) )
            {
                perror("send");
            }
              
            puts("Welcome message sent successfully");
              
            //add new socket to array of sockets
            for (i = 0; i < MAX_CLIENTS; i++)
            {
                //if position is empty
                if( client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n" , i);
                     
                    break;
                }
            }
        }
          
        //else its some IO operation on some other socket :)
        for (i = 0; i < MAX_CLIENTS; i++)
        {
            sd = client_socket[i];
              
            if (FD_ISSET( sd , &readfds))
            {
                //Check if it was for closing , and also read the incoming message
                if ((valread = read( sd , buffer, 1024)) == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(sd , (struct sockaddr*)&self , (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa
                    (self.sin_addr) , ntohs(self.sin_port));
                      
                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    client_socket[i] = 0;
                }
                  
                //Echo back the message that came in
                else
                {
                    //set the string terminating NULL byte on the end of the data read
                    buffer[valread] = '\0';
                    send(sd , buffer , strlen(buffer) , 0 );
                }
            }
        }
    }
      
    return 0;
}
