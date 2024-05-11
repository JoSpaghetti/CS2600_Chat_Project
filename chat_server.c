#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <pthread.h>

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

int AppendChatHistory(void* buffer) {
	// add mutexes later
	// initialize text
	char* text = (char *)buffer;
	// open file and write to it
	FILE* fp = fopen("chat_history", "a");
	fprintf(fp, "%s\n", text);
	printf("Written to logs");
	// close file and free pointers
	free(text);
	fclose(fp);
	pthread_exit(0);
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
		report("listen", 1); /* terminate */
	}

	fprintf(stderr, "Listening on port %i for clients...\n", PortNumber);
	
	// create thread for writing messages
	pthread_t write_thread;
	while (1) {
		struct sockaddr_in caddr; /* client address */
		unsigned int len = sizeof(caddr); /* address length could change */
		
		int client_fd = accept(fd, (struct sockaddr*) &caddr, &len); /* accept blocks */
		if (client_fd < 0) {
			report("accept", 0); /* don't terminate, though there's a problem */
			continue;
		}
		
		char* buffer = (char *)malloc(128 * sizeof(char));
		recv(client_fd, buffer, sizeof(buffer), 0); // receive message from client
		// make thread to write to file
		pthread_create(&write_thread, NULL, (void *)AppendChatHistory, (void *)buffer);
		printf("Response: %s\n", buffer);
		pthread_join(write_thread, 0);
	}
	return 0;
}
