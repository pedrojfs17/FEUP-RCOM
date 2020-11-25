#include "args.h"

int parseUrl(char * url, urlArgs * parsedUrl) {
    char * ftp = strtok(url, "/");
    char * args = strtok(NULL, "/");
    char * path = strtok(NULL, "/");

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
            parsedUrl->password = strdup("");
        else
            parsedUrl->password = password;
    }
    else {
        parsedUrl->user = strdup("");
        parsedUrl->password = strdup("");
        parsedUrl->host = args;
    }

    parsedUrl->path = path;

    if (parsedUrl->host == NULL || !strcmp(parsedUrl->host, "") || !strcmp(parsedUrl->path, "")) {
        fprintf(stderr, "Invalid URL!\n");
        return -1;
    }

    printf("Parsed URL:\n");
    printf("User: '%s'\n", parsedUrl->user);
    printf("Password: '%s'\n", parsedUrl->password);
    printf("Host: '%s'\n", parsedUrl->host);
    printf("Path: '%s'\n", parsedUrl->path);

    return 0;
}

int hasUser(char * args) {
    return strchr(args, '@') != NULL ? 1 : 0;
}