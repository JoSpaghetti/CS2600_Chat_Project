#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <pthread.h>

#define PortNumber 9876

int main() {
  //fd for the socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  //if (sockfd < 0) report ("socket", 1);
  if (sockfd < 0) {
    printf("Connected");
  }

  //gets the address of the host
  struct hostent* hptr = gethostbyname("127.0.0.1");
  if (!hptr) {
    //report ("gethostbyname", 1);
    printf("gethostbyname");
  }

  if (hptr->h_addrtype != AF_INET) {
    printf("bad address family");
    //report("bad address family", 1);
  }

  //connects to the server: configure server's address list
  struct sockaddr_in saddr;
  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = AF_INET;
  saddr.sin_addr.s_addr = ((struct in_addr*) hptr->h_addr_list[0])->s_addr;
  saddr.sin_port = htons(PortNumber);
  
  //connects to the sever
  if (connect(sockfd, (struct sockaddr*) &saddr, sizeof(saddr)) < 0) {
    //report("connect", 1);
    printf("connected\n");
  }
  
  printf("Success\n");
  close(sockfd);
}
