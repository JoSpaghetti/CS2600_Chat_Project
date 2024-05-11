#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <stdlib.h>
#define PortNumber 9876

void report(const char* msg, int status) {
	perror(msg);
	exit(status);
}

int main() {
  //fd for the socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) report ("socket", 1);

  //gets the address of the host
  struct hostent* hptr = gethostbyname("127.0.0.1");
  if (!hptr) {
    report ("gethostbyname", 1);
  }

  if (hptr->h_addrtype != AF_INET) {
    report("bad address family", 1);
  }

  //connects to the server: configure server's address list
  struct sockaddr_in saddr;
  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = AF_INET;
  saddr.sin_addr.s_addr = ((struct in_addr*) hptr->h_addr_list[0])->s_addr;
  saddr.sin_port = htons(PortNumber);
  
  //connects to the server
  if (connect(sockfd, (struct sockaddr*) &saddr, sizeof(saddr)) < 0) {
    report("connect", 1);
  }
  

	

	printf("Send message pls\n");
	char* line = NULL;
	size_t lineSize = 0;
	while(1) {
		// send message to server (start chatting)
		ssize_t charCount = getline(&line, &lineSize, stdin);
		if(charCount > 0) {
			if(strcmp(line, "exit\n") == 0) {
				break;
			}
		
		send(sockfd, line, charCount, 0);
		printf("Your message: %s\n", line);
		}
		free(line);
	}
	
	printf("Success\n");
  close(sockfd);
}
