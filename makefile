client: chat_client.c
	gcc -o client chat_client.c

server: chat_server.c
	gcc -o server chat_server.c

clean:
	rm -f client 
	rm -f server 
