#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

//define PortNumber 9876

int sockfd = 0;

//Checks to see if thread is working 1 means it is
int threadWorking = 1;

//Thread function that listens and prints the message from the server
void* print_message() {
  char msg[1024];
  //while true, keep receiving messages from the server
  while (1) {
    int receive = recv(sockfd, msg, 1024, 0);
    if (receive > 0) {
      //printf("Server said:\n");
      printf("%s", msg);
    } else {
      printf("Something went wrong with the message\n");
      break;
    }
    //makes msg empty again (i think)
    memset(msg, 0, sizeof(msg)); 
    if (threadWorking < 0) { //If they exited out, break out of the loop and exit
      break;
    }
  }
  //printf("Thread closed\n");
  pthread_exit("1");
}

void report(char* message, int status) {
  printf(message);
  exit(status);
}

int main(int argc, char* argv[]) {
  //Checks for correct aomunt of arguments
  //TODO: Maybe also check if its an integer
  if (argc != 2) {
    printf("Invalid amount of arguments. Make sure to include 1 portnumber");
    return -1;
  }

  int portNumber = atoi(argv[1]);
  //printf("%d", portNumber);
  //fd for the socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  //if (sockfd < 0) report ("socket", 1);
  if (sockfd < 0) {
    report("Connected", 0);
  }

  //gets the address of the host
  struct hostent* hptr = gethostbyname("127.0.0.1");
  if (!hptr) {
    report ("gethostbyname", 1);
    //printf("gethostbyname");
  }

  if (hptr->h_addrtype != AF_INET) {
    //printf("bad address family");
    report("bad address family", 1);
  }

  //connects to the server: configure server's address list
  struct sockaddr_in saddr;
  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = AF_INET;
  saddr.sin_addr.s_addr = ((struct in_addr*) hptr->h_addr_list[0])->s_addr;
  saddr.sin_port = htons(portNumber);
  
  pthread_t msgRecv;
  pthread_create(&msgRecv, NULL, print_message, NULL);
  
  //connects to the server
  if (connect(sockfd, (struct sockaddr*) &saddr, sizeof(saddr)) < 0) {
    //report("connect", 1);
    printf("not connected\n");
  }

  //Gets username from user
  char username[25];
  //size_t usernameSize = 0;
  printf("Enter a username: ");
  fgets(username, 25, stdin);
  username[strcspn(username, "\n")] = '\0';
  //printf("%s", username);
	  
  // send message to server (start chatting)
  char* line = NULL;
  size_t lineSize = 0;
  printf("Send message pls\n");
        
  //Username and message
  char totalMsg[1024];
  char colon[] = ": ";
  int cont = 1;
  memset(totalMsg, '\0', sizeof(totalMsg));

  while(cont > 0) {
    ssize_t charCount = getline(&line, &lineSize, stdin);
    //printf("%s", line);
    if(charCount > 0) {
      if(strcmp(line, "exit\n") == 0) {
        send(sockfd, line, charCount, 0);
	cont = -1;
	break;		
      }
      //Cats username to the msg
      strcat(totalMsg, username);
      strcat(totalMsg, colon);
      strcat(totalMsg, line);
      ssize_t newCharCount = sizeof(totalMsg);
      send(sockfd, totalMsg, newCharCount, 0);
      //ssize_t messageSent = send(sockfd, line, charCount, 0);
      memset(totalMsg, '\0', sizeof(totalMsg));
      charCount = 0;
    }
  }
  //Since done, close the thread
  threadWorking = -1;
  pthread_join(msgRecv, NULL);
  printf("Success\n");
  close(sockfd);
}
