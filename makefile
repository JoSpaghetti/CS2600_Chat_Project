client: chat_client.c
	gcc -o client chat_client.c
client_wall: chat_client.c
	gcc -o chat_client chat_client.c -w
server: chat_server.c
	gcc -o server chat_server.c
server_wall: chat_server.c
	gcc -o server chat_server.c -w
clean:
	rm -f client server 
