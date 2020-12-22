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
    
    return sockfd;
}

int login(int socketFd, char * username, char * password) {
	// Command USER
	if (sendCommand(socketFd, USER, TRUE, username) < 0) {
        fprintf(stderr, "Error sending USER command!\n");
        return -1;
    }

	int validResponses[2] = {CMD_USERNAME_OK, CMD_LOGIN_SUCCESS};

	if (checkResponseCodes(socketFd, validResponses, 2) < 0)
		return -1;


	// Command PASS
    if (sendCommand(socketFd, PASS, TRUE, password) < 0) {
        fprintf(stderr, "Error sending PASS command!\n");
        return -1;
    }

	if (checkResponseCode(socketFd, CMD_LOGIN_SUCCESS) < 0)
		return -1;

	return 0;
}

int passiveMode(int socketFd, pasvResponse * response) {
	socketResponse socketresponse;
	memset(&socketresponse, 0, sizeof(socketResponse));
	
	if (sendCommand(socketFd, PASV, FALSE, NULL) < 0) {
        fprintf(stderr, "Error sending PASV command!\n");
        return -1;
    }

	if (readResponse(socketFd, &socketresponse) < 0) {
        return -1;
	}

	if (socketresponse.code != CMD_PASV_MODE) {
		fprintf(stderr, "Response code failed!\n");
        return -1;
	}

	// Parse Response
	strtok(socketresponse.response, "(");
    char * args = strtok(NULL, ")");

	int ip[4], port[2];
	sscanf(args, "%d,%d,%d,%d,%d,%d", &ip[0], &ip[1], &ip[2], &ip[3], &port[0], &port[1]);

	response->port = port[0] * 256 + port[1];
	sprintf(response->ip, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

	return 0;
}

int sendCommand(int socketFd, char * command, int hasArgs, char * args) {
	char cmd[256];

	// Build Command
	buildCommand(command, hasArgs, args, cmd);

	// Send Command
	if (writeMessage(socketFd, cmd) < 0) {
        return -1;
	}

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

int readResponse(int socketFd, socketResponse * response) {
	FILE * socket = fdopen(socketFd, "r");

	char * buf;
	size_t bytesRead = 0;
	int totalBytesRead = 0;

	// Reads response line by line. Stops when the line is "<code> "
	while (getline(&buf, &bytesRead, socket) > 0) {
		strncat(response->response, buf, bytesRead - 1);
		totalBytesRead += bytesRead;

		if (buf[3] == ' ') {
			sscanf(buf, "%d", &response->code);
			break;
		}
    }

	free(buf);

	printf("< %s", response->response);

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

int checkResponseCode(int socketFd, int responseCode) {
	static socketResponse response;
	memset(&response, 0, sizeof(socketResponse));

	if (readResponse(socketFd, &response) < 0) {
        return -1;
	}	

	if (response.code != responseCode) {
		fprintf(stderr, "Response code failed!\n");
        return -1;
	}

	return 0;
}

int checkResponseCodes(int socketFd, int responseCodes[], int nCodes) {
	static socketResponse response;
	memset(&response, 0, sizeof(socketResponse));

	if (readResponse(socketFd, &response) < 0) {
        return -1;
	}	

	int valid = 0;

	for (int i = 0; i < nCodes; i++) {
		if (response.code == responseCodes[i])
			valid = 1;
	}

	if (!valid) {
		fprintf(stderr, "Response code failed!\n");
        return -1;
	}

	return 0;
}