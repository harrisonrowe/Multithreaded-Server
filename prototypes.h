#ifndef PROTOTYPES_H
#define PROTOTYPES_H

// Client prototypes
void initClient(struct sockaddr_in*, int*, int, char*);

// Server prototypes
void initServer(struct sockaddr_in*, int*, int);
void connectClient(struct sockaddr_in*, int*, int);

// Main function prototypes
void tokenString(char***, char*, int*);
void printLog(char*);
void printError(char*);

#endif