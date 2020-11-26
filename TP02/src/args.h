#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>

typedef struct {
    char * user;
    char * password;
    char * host;
    char * path;
    char * fileName;
    char * host_name;
    char * ip;
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