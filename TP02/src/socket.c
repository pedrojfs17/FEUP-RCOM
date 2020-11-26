#include "socket.h"

int initConnection(char * ip, int port) {
    int	sockfd;
	struct sockaddr_in server_addr;
	
	/*server address handling*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(port);
    
	/*open an TCP socket*/
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Error opening socket!\n");
        return -1;
	}

	/*connect to the server*/
	if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		fprintf(stderr, "Error connecting to server!\n");
        return -1;
	}

	char response[1024];
	bzero(response, 1024);
	if (readResponse(sockfd, response) < 0) {
        return -1;
	}	
    
    return sockfd;
}

int login(int socketFd, char * username, char * password) {
	char response[1024];

	if (sendCommand(socketFd, USER, TRUE, username, response) < 0) {
        fprintf(stderr, "Error sending USER command!\n");
        return -1;
    }

    if (sendCommand(socketFd, PASS, TRUE, password, response) < 0) {
        fprintf(stderr, "Error sending PASS command!\n");
        return -1;
    }

	return 0;
}

int passiveMode(int socketFd, pasvResponse * response) {
	char socketResponse[1024];
	
	if (sendCommand(socketFd, PASV, FALSE, NULL, socketResponse) < 0) {
        fprintf(stderr, "Error sending PASV command!\n");
        return -1;
    }

	// Parse Response
	strtok(socketResponse, "(");
    char * args = strtok(NULL, ")");

	int ip[4], port[2];
	sscanf(args, "%d,%d,%d,%d,%d,%d", &ip[0], &ip[1], &ip[2], &ip[3], &port[0], &port[1]);

	response->port = port[0] * 256 + port[1];
	sprintf(response->ip, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

	return 0;
}

int sendCommand(int socketFd, char * command, int hasArgs, char * args, char * response) {
	bzero(response, 1024);
	char cmd[256];

	// Build Command
	buildCommand(command, hasArgs, args, cmd);

	// Send Command
	if (writeMessage(socketFd, cmd) < 0) {
        return -1;
	}

	if (!strcmp(command, RETR)) return 0;

	// Wait for Response
	if (readResponse(socketFd, response) < 0) {
        return -1;
	}

	// TODO 
	// Parse Response to check if code is error code or not

	return 0;
}

int writeMessage(int socketFd, char * message) {
    int bytesSent;

    if ((bytesSent = write(socketFd, message, strlen(message))) != strlen(message)) {		
        fprintf(stderr, "Error writing message to socket!\n");
        return -1;
    }

	printf("> %s", message);

    return bytesSent;
}

int readResponse(int socketFd, char * response) {
	FILE * socket = fdopen(socketFd, "r");

	char * buf;
	size_t bytesRead = 0;
	int totalBytesRead = 0;

	// Reads response line by line. Stops when the line is "<code> "
	while (getline(&buf, &bytesRead, socket) > 0) {
		strncat(response, buf, bytesRead - 1);
		totalBytesRead += bytesRead;

		if (buf[3] == ' ')
			break;
    }

	free(buf);

	printf("< %s", response);

    return totalBytesRead;
}

void buildCommand(char * command, int hasArgs, char * args, char * cmd) {
	// Command
	strcpy(cmd, command);

	// Arguments
	if (hasArgs == TRUE) {
		strcat(cmd, " ");
		strcat(cmd, args);
	}

	// Command Terminator
	strcat(cmd, CRLF);
}