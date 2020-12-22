#include "file.h"

int downloadFile(urlArgs * args) {
    // Connect to socket
    int socketFd;

    if ((socketFd = initConnection(args->ip, 21)) < 0) {
        fprintf(stderr, "Error initializing connection!\n");
        return -1;
    }
    
    if (checkResponseCode(socketFd, CMD_SOCKET_READY) < 0)
        return -1;

    // Login
    if (login(socketFd, args->user, args->password) < 0) {
        fprintf(stderr, "Error in login!\n");
        return -1;
    }

    // Passive Mode
    pasvResponse r;
    if (passiveMode(socketFd, &r) < 0) {
        fprintf(stderr, "Error setting passive mode!\n");
        return -1;
    }

    int dataFd;
    if ((dataFd = initConnection(r.ip, r.port)) < 0) {
        fprintf(stderr, "Error initializing data connection!\n");
        return -1;
    }

    // Request file
    if (sendCommand(socketFd, RETR, TRUE, args->path) < 0) {
        fprintf(stderr, "Error sending PASV command!\n");
        return -1;
    }

    if (checkResponseCode(socketFd, CMD_RETR_READY) < 0)
        return -1;
    
    // Transfer file
    if (transferFile(dataFd, args->fileName) < 0) {
        fprintf(stderr, "Error transfering file!\n");
        return -1;
    }

    if (checkResponseCode(socketFd, CMD_TRANSFER_COMPLETE) < 0)
        return -1;

    // Close
    if (close(socketFd) < 0) {
        fprintf(stderr, "Error closing socket!\n");
        return -1;
    }

    if (close(dataFd) < 0) {
        fprintf(stderr, "Error closing data socket!\n");
        return -1;
    }

    return 0;
}

int transferFile(int dataFd, char * fileName) {
    int fileFd;

    if ((fileFd = open(fileName, O_WRONLY | O_CREAT, 0777)) < 0) {
        fprintf(stderr, "Error opening data file!\n");
        return -1;
    }

    char buf[1024];
    int numBytesRead;

    while((numBytesRead = read(dataFd, buf, 1024)) > 0) {
        if (write(fileFd, buf, numBytesRead) < 0) {
            fprintf(stderr, "Error writing data to file!\n");
            return -1;
        }
    }

    if (close(fileFd) < 0) {
        fprintf(stderr, "Error closing file!\n");
        return -1;
    }

    return 0;
}
