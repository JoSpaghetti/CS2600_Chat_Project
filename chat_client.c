#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <stdlib.h>
//define PortNumber 9876

int main(int argc, char *argv[]) {

  //Checks how many arguments
  if (argc != 2) {
    printf("Invalid amount of arguments. Make sure to include 1 portnumber\n");
    return -1;
  }
  
  //Sets first argument to portnumber
  int portNumber = atoi(argv[1]);
  printf("Portnumber chosen: %d\n", portNumber);

  //fd for the socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  //if (sockfd < 0) report ("socket", 1);
  if (sockfd < 0) {
    //error creating socket
    printf("Error");
    return 1;
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
  saddr.sin_port = htons(portNumber);
  
  //connects to the sever
  if (connect(sockfd, (struct sockaddr*) &saddr, sizeof(saddr)) < 0) {
    //report("connect", 1);
    printf("connected\n");
  } else {
    printf("not connected");
  }
  
  //Asks for username
  char tempName[25];
  printf("Enter your username:\n");
  scanf("%s", &tempName);
  char *username = (char*)malloc(sizeof(tempName)+1);
  username = tempName;
  
  //EXIT keyword
  char *exit = "EXIT_PROGRAM";
  printf("To exit, type %s\n", exit);
  
  //Sending Messages
  while(1) {
    char *msg;
    printf("Send a msg below:\n");
    scanf("%s", &msg);
 
    //Checks if user typed in exit keyword
    if ((strstr(msg, exit)) != NULL) {
      break;
    }
 
    //Sends the message
    send(sockfd, msg, strlen(msg), 0);
    
    char buffer[256];
    //Receive a message
    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer);
  }

  
  //free(username);
  //free(msg);
  //free(PortNumber);
  printf("Success\n");
  close(sockfd);
}
