#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <netinet/in.h>

void sigpipe_handler() {
  printf("Pipe to server is broken.");
}

void print_help() {
  fprintf(stderr, "Usage: ./client <ip> <port>\n");
}

int main(int argc, char *argv[]) {
  static char buffer[255];
  int sock_fd, port, err, length;
  struct sockaddr_in server_addr;
  signal(SIGPIPE, sigpipe_handler);
  bool done = false;
  if (argc != 3) {
    fprintf(stderr, "Invalid number of arguments!\n");
    print_help();
    return 1;
  }
  // parse port
  if (sscanf(argv[2], "%d", &port) != 1) {
    fprintf(stderr, "Can't parse port: '%s'\n", argv[2]);
    print_help();
    return 1;
  }
  // open socket
  sock_fd = socket(PF_INET, SOCK_STREAM, 0);
  if (sock_fd == -1) {
    fprintf(stderr, "Can't create new socket!\n");
    return 1;
  }
  // prepare connect
  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port   = htons(port);
  if (inet_aton(argv[1], &server_addr.sin_addr.s_addr) == 0) {
    fprintf(stderr, "Invalid address '%s'.\n", argv[1]);
    return 1;
  }
  // connect to server
  err = connect(sock_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));
  if (err != 0) {
    fprintf(stderr, "Couldn't connect to %s:%d.\n", argv[1], port);
    return 1;
  }
  while (!done) {
    printf(" > ");
    bzero(buffer, sizeof(buffer));
    fgets(buffer, sizeof(buffer), stdin);
    length = strlen(buffer);
    if (send(sock_fd, buffer, length, 0) != length) {
      fprintf(stderr, "Couldn't write into socket.\n");
      return 1;
    }
    err = recv(sock_fd, buffer, sizeof(buffer), 0);
    if (err == 0) {
      fprintf(stderr, "Couldn't read from socket.\n");
      return 1;
    }
    puts(buffer);
  }
  close(sock_fd);
  return 0;
}
