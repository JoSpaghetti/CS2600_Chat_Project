client: chat_client.c
	gcc -o client chat_client.c -Wall -lpthread

server: chat_server.c
	gcc -o server chat_server.c -Wall -lpthread

clean:
	rm -f client 
	rm -f server 
