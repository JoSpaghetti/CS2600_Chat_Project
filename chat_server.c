#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

/* define things for network sockets */
#define PortNumber 9876
#define MaxConnects 8
#define BuffSize 256
#define ConversationLen 3
#define Host "localhost"

void report(const char* msg, int status) {
	perror(msg);
	exit(status);
}

void fileWrite(int isOpen) {
	FILE *filep;
	filep = fopen("chat_history", "w");
	
	//the following code gets the current time and formats it Day, Month, Year
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	
	char message[500]; 
	printf ( "%s %s", asctime (timeinfo), message );
	fprintf(filep, message);

	//code idea
	//	in main, we use pthread_create() to call this function
	//	the function then opens the file whenever it needs to write a comment
	//	We use an int to check if the file has already been accessed so we can wipe the information
	//	ex. int isOpen functions like a boolean saying whether the file has been opened, or if it needs to write over the past information
	//	The file is locked using mutex??? while the program runs
	//	The file closes untill the next comment is sent
	//better code idea
	//	the file is opened in main, and we use fprintf in the method to write messages into the file chat_history
	//	-however, threads work with functions and we can't just make fprintf a method in issolation
	//	--maybe we could make an inner class inside of main and it calls the inner class
	//	--idk, this is all a thought
}


int main() {
	/* network vs AF_LOCAL */
	/* reliable, bidirectional, arbitrary payload size */
	/* system picks underlying protocol (TCP) */
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	/* terminate */
	if (fd < 0) report("socket", 1);
	
	/* bind the server's local address in memory */
	struct sockaddr_in saddr; /* clear the bytes */
	saddr.sin_family = AF_INET; /* vs AF_LOCAL */
	saddr.sin_addr.s_addr = htonl(INADDR_ANY); /* host-to-network endian */
	saddr.sin_port = htons(PortNumber); /* for listening */
	
	if (bind(fd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
		report("bind", 1); /* terminate */
	}
	
	/* listen to the socket */
	if (listen(fd, MaxConnects) < 0) { /* listen for clients, up to MaxConnects */
		report("listen", 1);	/* terminate */
	}
	
	fprintf(stderr, "Listening on port %i for clients...\n", PortNumber);
	fileWrite();
	while (1) {
		struct sockaddr_in caddr; /* client address */
		int len = sizeof(caddr); /* address length could change */
		
		int client_fd = accept(fd, (struct sockaddr*) &caddr, &len); /*accept blocks */
		if (client_fd < 0) {
			report("accept", 0); /* don't terminate, though there's a problem */
			continue;
		}
		// put the read/ write stuff from client
	}
	
}
