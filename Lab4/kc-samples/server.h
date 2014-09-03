/* Kristin Onorati
*  10874126
*  Group 1 Seq 3
*  CPTS 360 Lab 4
*/

#ifndef SERVER_H
#define SERVER_H
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include "mystat.h"



#define NUM_CMNDS 9

// Define variables:
struct sockaddr_in  server_addr, client_addr, name_addr;
struct hostent *hp;

int  sock, newsock;                  // socket descriptors
int  serverPort;                     // server port number
int  r, length, n;                   // help variables

int server_init(char *name);
char** parseInput(char* input);
void runProg(char**input, int sock);
int getFile(char* filename, int sock);
int putFile(char* filename, int sock);



#endif
