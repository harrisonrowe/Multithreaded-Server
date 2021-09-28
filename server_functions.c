#include "includes.h"

// Create and establish a server process and await for an incomming connection
void initServer(struct sockaddr_in* server, int* serverSocket, int port){
    // Print init message to server
    printf("\n");
    printLog("2803ICT Assignment 1 - Milestone 2: By Harry Rowe.");
    printLog("Initialising server...");
    // CREATE SOCKET
    // Create TCP socket (using address family and socket stream)
    *serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    // Check if socket unavailible
    if (*serverSocket == -1){
        printError("Socket failed");
    } else {
        printLog("Socket created.");
    }
    // Prepare sockaddr structure
    server->sin_family = AF_INET;
    server->sin_addr.s_addr = INADDR_ANY; // Current IP address
    server->sin_port = htons(port); // Port number specified as macro
    // BIND ADDR TO SOCKET
    // Bind address
    int bindRes = bind(*serverSocket, (struct sockaddr *) &*server, sizeof(*server));
    // Error handle for bind error
    if (bindRes < 0){
        printError("Bind failed.");
    } else {
        printLog("Bind successful.");
    }
    // LISTEN FOR CLIENT
    int listenRes = listen(*serverSocket, BACKLOG);
    // Error handle for waiting for client
    if (listenRes != 0){
        printError("Listen failed.");
    } else {
        printLog("Waiting for a client to join...");
    }
}

// Connect a client to the server
void connectClient(struct sockaddr_in* client, int* clientSocket, int serverSocket){
    // Accept connection from client
    int clientSize = sizeof((*client));
    (*clientSocket) = accept(serverSocket, (struct sockaddr *) &*client, &clientSize);
    // Check if connection is succesful
    if ((*clientSocket) < 0){
        printError("Connection unsuccessful.");
    } else {
        printLog("User has connected!");
    }
}