all: game_client game_server

game_client: game_client.c client_functions.c functions.c
	gcc -o game_client game_client.c client_functions.c functions.c

game_server: game_server.c server_functions.c functions.c
	gcc -o game_server game_server.c server_functions.c functions.c