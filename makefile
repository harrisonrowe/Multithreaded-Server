all: client server

client: client.c client_functions.c functions.c stack.c
	gcc -o client client.c client_functions.c functions.c stack.c

server: server.c server_functions.c functions.c stack.c
	gcc -o server server.c server_functions.c functions.c stack.c