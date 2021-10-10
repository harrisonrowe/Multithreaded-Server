#ifndef PROTOTYPES_H
#define PROTOTYPES_H

// Client prototypes
void* slotConsumer(void*);
unsigned int validateClientInput(char*);

// Server prototypes
void* factorise(void*);
unsigned int rightRotate(unsigned int, unsigned int);

// Stack prototypes
struct Stack* stackInit(int);
int stackFull(struct Stack*);
int stackEmpty(struct Stack*);
void stackPush(struct Stack*, int);
int stackPop(struct Stack*);

// Main function prototypes
void printLog(char*);
void printError(char*);

#endif