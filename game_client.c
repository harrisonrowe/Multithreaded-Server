#include "includes.h"

int main(int argc, char* argv[]){
    // Validate command line input
    if (argc != 3){
        printError("Insuffficient command line arguments.");
    }

    // Client socket structure
    struct sockaddr_in server;
    // Used for bind of client socket
    int res = 0;
    // Hold client socket descriptor entry
    int clientSocket = 0;
    // Port/server variables
    int port = atoi(argv[1]);
    // Command line variables
    char serverName[SIZE];
    // Copy from command line
    strcpy(serverName, argv[2]);

    // Initiate client
    initClient(&server, &clientSocket, port, serverName);

    // If communication with server is successful, run a loop, listening for messages
    while(1){

        // Request user for 32 bit integer
        char clientMessage[SIZE] = "";
        printLog("Please enter integer:");
        printf("> ");
        scanf("%[^\n]%*c", clientMessage);
        printf("\n");

        // Send data to server 
        int sendRes = send(clientSocket, clientMessage, SIZE, 0);
        if (sendRes < 0){
            printError("Send data error.");
        }
        
        // Check if client wishes to quit
        if (strcmp(clientMessage, "q") == 0){
            printLog("Disconnecting from server...");
            break;
        }

    }

    // Close client socket
    close(clientSocket);
    return 0;
}