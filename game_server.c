// cd Assignment2/Multithreaded-Server
// ./game_server 80
// ./game_client 80 HARRY-PC2
#include "includes.h"

int main(int argc, char* argv[]){
    // Validate command line input
    if (argc != 2){
        printError("Insuffficient command line arguments.");
    }

    // Check port address
    if (!isdigit(*argv[1]) || atoi(argv[1]) < 1){
        printError("Please enter a valid port number.");
    }

    // Client and server messages
    char clientMsg[SIZE] = "";
    char serverMsg[SIZE] = "";
    // Client and server sockets
    struct sockaddr_in client, server;
    // Hold client and server socket descriptor entry
    int clientSocket = 0, serverSocket = 0;
    // Child process ID
    pid_t cpid, listenid;
    // Port/server variables
    int port = atoi(argv[1]);

    // Initiate server connection
    initServer(&server, &serverSocket, port);
    // Accept connection - Accept 1 client
    connectClient(&client, &clientSocket, serverSocket);

    // Server initiated
    while(1){
        
        // Await integer from client
        char clientMessage[SIZE] = "";
        int recvRes = recv(clientSocket, clientMessage, SIZE, 0);
        if (recvRes < 0){
            printError("Cannot recieve data from client");
        }

        // Print recv from client
        printLog("Recieved from client: ");
        printf("\tClient = %s\n\n", clientMessage);

        // Check if client wishes to quit
        if (strcmp(clientMessage, "q") == 0){
            printLog("Client disconnecting...");
            break;
        }

    }
    
    // Close client socket
    close(clientSocket);
    // Close server socket
    printLog("Server closing.");
    close(serverSocket);
    return 0;
}