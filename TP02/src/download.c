#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>

#include "file.h"

int main(int argc, char * argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: download ftp://[<user>:<password>@]<host>/<url-path>\n");
        exit(1);
    }

    urlArgs url;

    if (parseUrl(argv[1], &url) < 0) {
        fprintf(stderr, "Error parsing url!\n");
        exit(2);
    }

    if (downloadFile(&url) < 0) {
        fprintf(stderr, "Error downloading file!\n");
        exit(3);
    }

    return 0;
}
