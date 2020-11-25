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

int sendMessage(int socketFd, char * message) {
    int bytesSent;

    if ((bytesSent = write(socketFd, message, strlen(message))) != strlen(message)) {		
        fprintf(stderr, "Error sending message to socket!\n");
        return -1;
    }

    return bytesSent;
}