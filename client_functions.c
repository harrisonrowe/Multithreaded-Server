#include "includes.h"

// Initialise client and connect to specific server
void initClient(struct sockaddr_in* server, int* clientSocket, int port, char* host){
    // Print init message to clients
    printLog("Initialising client...");
    // CREATE SOCKET
    // Get IP address of server name (hostname) the client wishes to connect to
    struct hostent *host_entry;
    char* ip;
    host_entry = gethostbyname(host);
    ip = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0])); // Returns IP address of host
    // Prepare socket structure
    server->sin_addr.s_addr = inet_addr(ip);
    server->sin_family = AF_INET;
    server->sin_port = htons(port);
    // Create socket descriptor
    *clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    // Error handle for socket
    if (*clientSocket < 0){
        printError("Socket failed.");
    } else {
        printLog("Socket created.");
    }
    // CONNECT TO SERVER
    int connectRes = connect(*clientSocket, (struct sockaddr *) &*server, sizeof(*server));
    // Check if connection successful
    if (connectRes == -1){
        printError("Connection failed.");
    } else {
        printLog("Connected to server.");
    }
}

// Validate that the client has input a 32 bit signed integer
unsigned int validateClientInput(char clientMessage[SIZE]){
    // Check each character in string
    for (int i = 0; clientMessage[i] != '\0'; i++){
        if (i == 0 && clientMessage[i] == '-'){
            // Negative number entered
            return 0;
        } else if (!isdigit(clientMessage[i])){
            // Character is not a number
            return 0;
        }
    }
    // Ensure value assigned to n is between 1 and UINT max
    unsigned int n = atoi(clientMessage);
    if (n > UINT_MAX || n < 1){
        return 0;
    }
    // String validated, accept client input
    return n;
}