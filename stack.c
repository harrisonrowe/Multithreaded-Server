#include "includes.h"

// Stack functions

// Create stack
struct Stack* stackInit(int cap){
    // Init stack
    struct Stack* s = (struct Stack*) malloc(sizeof(struct Stack));
    // Set stack attributes
    s->cap = cap;
    s->top = -1;
    s->stack = (int*) malloc(cap*sizeof(int));
    // Return stack
    return s;
}

// Check if stack is full
int stackFull(struct Stack* s){
    return s->top == s->cap-1;
}

// Check if stack is empty
int stackEmpty(struct Stack* s){
    return s->top == -1;
}

// Add item to top of stack
void stackPush(struct Stack* s, int n){
    // Check is stack is full
    if (stackFull(s)){
        printError("Push - stack slot number out of bounds");
    }
    // Add item to top of stack
    s->stack[++s->top] = n;
}

// Remove item from top of stack
int stackPop(struct Stack* s){
    // Check if stack is empty
    if (stackEmpty(s)){
        printError("Pop - stack slot number out of bounds");
    }
    // Return top element
    return s->stack[s->top--];
}
