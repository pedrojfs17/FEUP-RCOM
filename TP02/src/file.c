#include "file.h"

int downloadFile(urlArgs * args) {
    // Connect to socket
    int socketFd;
    if ((socketFd = initConnection(args->ip, 21)) < 0) {
        fprintf(stderr, "Error initializing connection!\n");
        return -1;
    }

    // Login


    // Passive Mode


    // Request file


    // Copy file


    // Close
    if (close(socketFd) < 0) {
        fprintf(stderr, "Error closing socket!\n");
        return -1;
    }

    return 0;
}