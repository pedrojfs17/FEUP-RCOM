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
    char * host_name;
    char * ip;
} urlArgs;

int parseUrl(char * url, urlArgs * parsedUrl);

int hasUser(char * args);