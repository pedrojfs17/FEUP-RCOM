#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>

typedef struct {
    char host[512];
    char user[256];
    char password[256];
    char path[512];
    char fileName[512];
    char host_name[512];
    char ip[20];
} urlArgs;

/**
 * Parsed the url received as argument
 */
int parseUrl(char * url, urlArgs * parsedUrl);

/**
 * Retrieves filename from path
 */
char * getFilename(char * path);

/**
 * Checks if the url has username and password
 */
int hasUser(char * args);