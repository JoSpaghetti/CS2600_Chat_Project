#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
/* define things for network sockets */
#define PortNumber 9876
#define MaxConnects 8
#define BuffSize 256
#define ConversationLen 3
#define Host "localhost"

pthread_mutex_t writing_mutex = PTHREAD_MUTEX_INITIALIZER;

void report(const char* msg, int status) {
	perror(msg);
	exit(status);
}

void *AppendChatHistory(void* buffer) {
	// add mutexes later
	pthread_mutex_lock(&writing_mutex);
	// initialize text
	char* text = (char *)buffer;
	// open file and write to it
	FILE* fp = fopen("chat_history", "a");
	fprintf(fp, "%s", text);
	fprintf(stderr, "Written to logs");
	// close file
	fclose(fp);
	pthread_mutex_unlock(&writing_mutex);
	pthread_exit(0);
}

int main() {
	/* network vs AF_LOCAL */
	/* reliable, bidirectional, arbitrary payload size */
	/* system picks underlying protocol (TCP) */
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	// create thread for writing messages
	pthread_t write_thread;
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
	
	while (1) {
		struct sockaddr_in caddr; /* client address */
		unsigned int len = sizeof(caddr); /* address length could change */
		
		int client_fd = accept(fd, (struct sockaddr*) &caddr, &len); /* accept blocks */
		if (client_fd < 0) {
			report("accept", 0); /* don't terminate, though there's a problem */
			continue;
		}
	
		// listen for client and write to logs
		int result;
		char* buffer = (char *)malloc(1024 * sizeof(char));
		while(1) {
			recv(client_fd, buffer, sizeof(buffer), 0);
			printf("Received: %s", buffer);
			// make thread to write to file
			if ((result = pthread_create(&write_thread, NULL, AppendChatHistory, (void *)buffer) != 0)) {
				printf("Thread creation failed %d\n", result);
			}
			pthread_join(write_thread, NULL);
		}
	}
	return 0;
}
