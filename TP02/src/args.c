#include "args.h"

int parseUrl(char * url, urlArgs * parsedUrl) {
    char * ftp = strtok(url, "/");
    char * args = strtok(NULL, "/");
    char * path = strtok(NULL, "");

    if (ftp == NULL || args == NULL || path == NULL) {
        fprintf(stderr, "Invalid URL!\n");
        return -1;
    }

    if (hasUser(args)) {
        char * login = strtok(args, "@");
        parsedUrl->host = strtok(NULL, "@");

        parsedUrl->user = strtok(login, ":");
        char * password = strtok(NULL, ":");

        if (password == NULL)
            parsedUrl->password = strdup("pass");
        else
            parsedUrl->password = password;
    }
    else {
        parsedUrl->user = strdup("anonymous");
        parsedUrl->password = strdup("pass");
        parsedUrl->host = args;
    }

    parsedUrl->path = path;
    parsedUrl->fileName = getFilename(path);

    if (parsedUrl->host == NULL || !strcmp(parsedUrl->host, "") || !strcmp(parsedUrl->path, "")) {
        fprintf(stderr, "Invalid URL!\n");
        return -1;
    }

    printf("User: %s\n", parsedUrl->user);
    printf("Password: %s\n", parsedUrl->password);
    printf("Host: %s\n", parsedUrl->host);
    printf("Path: %s\n", parsedUrl->path);
    printf("File name: %s\n", parsedUrl->fileName);

    struct hostent * h;

    if ((h = gethostbyname(parsedUrl->host)) == NULL) {  
        herror("gethostbyname");
        return -1;
    }

    parsedUrl->host_name = h->h_name;
    parsedUrl->ip = inet_ntoa(*((struct in_addr *)h->h_addr));

    printf("Host name  : %s\n", parsedUrl->host_name);
    printf("IP Address : %s\n", parsedUrl->ip);

    return 0;
}

char * getFilename(char * path) {
    char * filename = path, *p;
    for (p = path; *p; p++) {
        if (*p == '/' || *p == '\\' || *p == ':') {
            filename = p + 1;
        }
    }
    return filename;
}

int hasUser(char * args) {
    return strchr(args, '@') != NULL ? 1 : 0;
}