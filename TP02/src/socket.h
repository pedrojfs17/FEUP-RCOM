#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <errno.h> 
#include <netdb.h> 
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>

#include "RFC.h"

#define TRUE 1
#define FALSE 0

typedef struct {
    char * ip;
    int port;
} pasvResponse;

/**
 * Starts the connection with a socket
 */
int initConnection(char * ip, int port);

/**
 * Login routine
 */
int login(int socketFd, char * username, char * password);

/**
 * Enter in passive mode
 */
int passiveMode(int socketFd, pasvResponse * response);

/**
 * Send a command to the socket
 */
int sendCommand(int socketFd, char * command, int hasArgs, char * args, char * response);

/**
 * Writes a message to the socket
 */
int writeMessage(int socketFd, char * message);

/**
 * Read response from socket
 */
int readResponse(int socketFd, char * response);

/**
 * Builds a command
 */
void buildCommand(char * command, int hasArgs, char * args, char * cmd);