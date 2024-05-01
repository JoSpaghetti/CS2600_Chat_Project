CC=gcc
client: chat_client.c
	$(CC) -o client -w chat_client.c -lpthread

server: chat_server.c
	$(CC) -o server -w chat_server.c -lpthread

clean:
	rm -f client server 
