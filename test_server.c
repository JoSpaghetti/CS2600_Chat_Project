#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PortNumber 20015
#define MaxConnects 8
#define BuffSize 256
#define ConversationLen 50
#define Host "localhost"
#define HistoryFile "chat_history"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int clients[20];
int amt_clients = 0;
void report(const char *msg, int terminate) {
    perror(msg);
    if (terminate) exit(EXIT_FAILURE);
}

void *handle_client(void *arg) {
    int client_fd = *((int *)arg);
    char buffer[BuffSize];
        clients[amt_clients] = client_fd;
        amt_clients += 1;
    // Lock the mutex before accessing shared resource
    //pthread_mutex_lock(&mutex);


    // Open the chat history file in append mode
    /*
    FILE *history_file = fopen(HistoryFile, "a");
    if (history_file == NULL) {
        perror("Error opening chat history file");
        pthread_mutex_unlock(&mutex);
        close(client_fd);
        return NULL;
    }*/

    char* name = (char*)malloc(20);
    read(client_fd, name, sizeof(name));

       /* read from client */
    while(1){
        memset(buffer, '\0', sizeof(buffer));
        int count = read(client_fd, buffer, sizeof(buffer));
        char formatted_buffer[300];
        if (count > 0) {
                pthread_mutex_lock(&mutex);
                FILE *history_file = fopen(HistoryFile, "a");
                if (history_file == NULL) {
                        perror("Error opening chat history file");
                pthread_mutex_unlock(&mutex);
                close(client_fd);
                return NULL;
                }
            fprintf(history_file, "%s:%s\n",name, buffer);

            fflush(history_file);
            for(int i = 0; i < amt_clients; i ++){
                    if(clients[i] != client_fd){


                        write(clients[i], buffer, count);
                }
            }

            puts(buffer);
            fclose(history_file);
            pthread_mutex_unlock(&mutex);
        }else if(count == 0){
                break;
        }else {
                perror("error reading from client");
                break;
        }
}
        free(name);
    //fclose(history_file);

    // Unlock the mutex after finishing critical section
    //pthread_mutex_unlock(&mutex);

    // Close the connection with the client
    close(client_fd);
    return NULL;
}

int main(){
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0){
                report("socket", 1);
        }

        struct sockaddr_in saddr;
        memset(&saddr, 0 ,sizeof(saddr));
        saddr.sin_family = AF_INET;
        saddr.sin_addr.s_addr = htonl(INADDR_ANY);
        saddr.sin_port = htons(PortNumber);

        if(bind(fd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0){
                report("bind", 1);
        }

        if(listen(fd, MaxConnects) < 0){
                report("listen", 1);
        }

        fprintf(stderr, "Listening on port %i for clients...\n", PortNumber);

        while (1){
                struct sockaddr_in caddr;
                socklen_t len = sizeof(caddr);


                int client_fd = accept(fd, (struct sockaddr *) &caddr, &len);


                if (client_fd < 0){
                        report("accept", 0);
                        continue;
                }
                 pthread_t tid;
                 if (pthread_create(&tid, NULL, handle_client, &client_fd) != 0) {
                         perror("pthread_create");
                         close(client_fd);
                 }
        }

        return 0;
}
