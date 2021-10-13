# Multithreaded-Server

This project contains two executable files: one client and one server.

The purpose was to implement a multithreaded client and server which takes in upto 10 unsigned integer requests from the client and processes factors derived from the number as well as 32 other numbers (the unsigned int rotated right by x amount of bits). Each request uses multithreading to find factors as quickly as possible.

# Compilation

Ensure the system is either Linux (UNIX) or MacOS. If windows is used, ensure Cygwin or a virtual machine is installed.

Once in the directory, type the following in the terminal:

1) 'make' -> Compiles the .c files into two executables: server.exe and client.exe
2) './server' -> Ensure this is executed first (for shared memory)
3) './client' -> Runs client and connects to server through virtual memory
4) Type requests to the client (unsigned 32 bit integers) to be processed by the server. A maximum of 10 requests can be taken at a time.