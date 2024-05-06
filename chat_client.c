#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <pthread.h>

//define PortNumber 9876

//Thread function that listens and prints the message from the server
//TODO: Check if this works
void *print_message(void *ptr) {
  int *sockfd;
  sockfd = (int *) ptr;
  char msg[1024];
  //while true, keep receiving messages from the server
  while (1) {
    int receive = recv(sockfd, msg, 1024, 0);
    if (receive > 0) {
      printf("%s", buffer);
    } else {
      break;
    }
    //makes msg empty again (i think)
    memset(msg, 0, sizeof(msg)); 
  }
}

int main(int argc, char* argv) {
  //Checks for correct aomunt of arguments
  //TODO: Maybe also check if its an integer
  if (argc != 2) {
    printf("Invalid amount of arguments. MAke sure to include 1 portnumber");
    return -1;
  }

  int PortNumber = atoi(argv[1]);

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
  
  //connects to the server
  if (connect(sockfd, (struct sockaddr*) &saddr, sizeof(saddr)) < 0) {
    //report("connect", 1);
    printf("connected\n");
  }
  	//Gets username from user
  	//TODO: Maybe change this so that everytime you send a message, the username goes first
  	//TODO: Example - Jairus: Hey guys its me Jairus
  	char *username = NULL;
	size_t usernameSize = 0;
  	printf("Enter a username:");
        ssize_t nameCount = getline(&username, &usernameSize, stdin);

	// send message to server (start chatting)
	char* line = NULL;
	size_t lineSize = 0;
	printf("Send message pls\n");

	//Create thread to receive messages
	pthread_t thread;
	pthread_create(&thread, NULL, print_message, (void *) sockfd);

	while(1) {
		ssize_t charCount = getline(&line, &lineSize, stdin);
		if(charCount > 0) {
			if(strcmp(line, "exit\n") == 0) {
				break;
			}
		//sends username then message
		ssize_t usernameSent = send(sockfd, username, nameCount, 0);
		ssize_t messageSent = send(sockfd, line, charCount, 0);

		}
	}

	//Closes the thread
  pthread_detach(thread);
  printf("Success\n");
  close(sockfd);
}
