#ifndef PROTOTYPES_H
#define PROTOTYPES_H

// Client prototypes
void printProgress();
unsigned int validateClientInput(char*);
void* slotConsumer(void*);
void* testConsumer(void* args);


// Server prototypes
unsigned int rightRotate(unsigned int, unsigned int);
void* factorise(void*);
void* testmode(void*);


// Main function prototypes
void printLog(char*);
void printError(char*);

#endif