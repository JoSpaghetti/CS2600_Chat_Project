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
#define PortNumber 7029
#define MaxConnects 7
#define Host "localhost"

pthread_mutex_t writing_mutex = PTHREAD_MUTEX_INITIALIZER;
int clients[MaxConnects];
char buffer[1024];

void report(const char* msg, int status) {
	perror(msg);
	exit(status);
}

void *ReadClient(void * client) {
	int *client_fd = (int *)client;
	while(1) {
		memset(buffer, '\0', sizeof(buffer));
		int count = recv(*client_fd, buffer, sizeof(buffer), 0);
		if (count > 0) {
			puts(buffer);
			// begin writing to file
			pthread_mutex_lock(&writing_mutex);
			// initialize text
			char* text = buffer;
			// open file and write to it
			FILE* fp = fopen("chat_history", "a");
			fprintf(fp, "%s", text);
			// close file
			fclose(fp);
			pthread_mutex_unlock(&writing_mutex);
			
			//send to other clients
			for (int i = 0; i < MaxConnects; i++) {
				printf("Finding client %d, %d\n", i, clients[i]);
				if (clients[i] != *client_fd) {	
					printf("sent to: %d\n", clients[i]);
					send(clients[i], buffer, sizeof(buffer), 0);
				}
			}
		}
		// close the client?
		close(*client_fd);
	}
	pthread_exit(0);
}

// initialize client array
int addClient(int client) {
	int isThere = 0;
	int index = 0;
	for(int j = 0; j < MaxConnects; j++) {
		if (clients[j] == client) {
			isThere = 1;
		}	
	}
	if (isThere == 0) {
		for(int i = 0; i < MaxConnects; i++) {
			if(!clients[i]){
				clients[i] = client;
				index = i;
				break;
			}
		}
	}
	return index;
}

int main() {
	/* network vs AF_LOCAL */
	/* reliable, bidirectional, arbitrary payload size */
	/* system picks underlying protocol (TCP) */
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	int index = 0;
	// create thread for writing messages
	//pthread_t write_thread;
	pthread_t read_threads[MaxConnects];	
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

		if (clients[MaxConnects-1]) {
			report("Max numbers of clients reached\nTry again later.", 1);
		} 

		index = addClient(client_fd);
		printf("Added client: %d\n", client_fd);
		
		// listen for client messages and write to logs
		for (int i = 0; i < MaxConnects; i++) {
			pthread_create(&read_threads[i], NULL, ReadClient, (void *) &clients[i]);
		}
		//pthread_creaate(&read_threads[index], NULL, ReadClient, (void *) &client_fd);
	}
	return 0;
}
