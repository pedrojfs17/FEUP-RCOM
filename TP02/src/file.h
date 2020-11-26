#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <errno.h> 
#include <netdb.h> 
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>

#include "args.h"
#include "socket.h"

/**
 * Downloads a file with FTP
 */
int downloadFile(urlArgs * args);

/**
 * Read data and writes to the created file
 */
int transferFile(int dataFd, char * fileName);