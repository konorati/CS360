/* Kristin Onorati
*  10874126
*  Group 1 Seq 3
*  CPTS 360 Lab 4
*/

#ifndef CLIENT_H
#define CLIENT_H

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include "mystat.h"

// Define variables
struct hostent *hp;              
struct sockaddr_in  server_addr; 

int sock, r;
int SERVER_IP, SERVER_PORT; 


int client_init(char *argv[]);
char **parseInput(char* input);
int getFile(char* filename, int sock);
int putFile(char* filename, int sock);
int runProg(char** input, int cmnd);
int cat(char* filename);


#endif
