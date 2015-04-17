#include <stdio.h>
#include <strings.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENTS 3

void* connection_handler(int sock) {
  int err;
  char buffer[255];  
  bzero(buffer, 255);
  err = read(sock, buffer, 255);
  if (err == 0) {
    fprintf(stderr, "Couldn't read! %d", sock);
    // exit thread
  }
}

void print_help() {
  fprintf(stderr, "Usage: ./server <port>\n");
}

int main(int argc, char *argv[]) {
  int err, sock_fd, port;
  bool done = false;
  struct sockaddr_in self;
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
  // open socket
  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd == -1) {
    fprintf(stderr, "Can't create new socket!\n");
    return 1;
  }
  // prepare bind
  bzero(&self, sizeof(self));
  self.sin_family = AF_INET;
  self.sin_port   = htons(port);
  self.sin_addr.s_addr = INADDR_ANY;
  err = bind(sock_fd, (struct sockaddr*) &self, sizeof(self));
  if (err != 0) {
    fprintf(stderr, "Couldn't bind port %d.\n", port);
    return 1;
  }
  if (listen(sock_fd, 20) != 0) {
    fprintf(stderr, "Couldn't make socket, to listening socket.");
    return 1;
  }
  while (!done) {
    int client_fd;
    struct sockaddr_in client_addr;
    int addrlen = sizeof(client_addr);
    client_fd = accept(sock_fd, (struct sockaddr*) &client_addr, &addrlen);
    struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&client_addr;
    int ipAddr = pV4Addr->sin_addr.s_addr;
    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);
    printf("%s:%d connected.\n", str, client_addr.sin_port);
    // create new thread for new client
  }
  return 0;
}
