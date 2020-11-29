#include "file.h"

int downloadFile(urlArgs * args) {
    // Connect to socket
    int socketFd;

    if ((socketFd = initConnection(args->ip, 21)) < 0) {
        fprintf(stderr, "Error initializing connection!\n");
        return -1;
    }

    /* 
        Se eu chamar esta funcao checkResponse (que faz o mesmo que o
        codigo que esta em baixo destes comentarios) ele altera o 
        valor do args->path dentro da funcao passiveMode mesmo nao sendo 
        enviado o pointer args para la. Nao percebo porque e que isso 
        acontece, talvez seja algum problema de pointers, nao sei, por 
        enquanto fica assim que funciona.
    */
    /*
    if (checkResponse(socketFd, CMD_SOCKET_READY) < 0)
        return -1;
    */

    socketResponse response;
	memset(&response, 0, sizeof(socketResponse));
	if (readResponse(socketFd, &response) < 0) {
        return -1;
	}	

	if (response.code != CMD_SOCKET_READY) {
		fprintf(stderr, "Response code failed!\n");
        return -1;
	}

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
    
    // Transfer file
    if (transferFile(dataFd, args->fileName) < 0) {
        fprintf(stderr, "Error transfering file!\n");
        return -1;
    }

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
