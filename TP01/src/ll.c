/* Application layer functions */

#include "ll.h"

struct termios oldtio;
struct linkLayer ll;

int llopen(int port, int role) {
    sprintf(ll.port, "/dev/ttyS%d", port);

    int fd = open(ll.port, O_RDWR | O_NOCTTY);
    if (fd < 0) { 
        perror(ll.port);
        return -1;
    }

    struct termios newtio;

    if (tcgetattr(fd, &oldtio) == -1) { /* save current port settings */
        perror("tcgetattr");
        return -1;
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
 
    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;
 
    newtio.c_cc[VTIME]    = (role)? 0 : 1;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = (role)? 1 : 0;   /* blocking read until 5 chars received */
 
    tcflush(fd, TCIOFLUSH);
 
    if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
        perror("tcsetattr");
        return -1;
    }
 
    printf("New termios structure set\n");

    signal(SIGALRM, atende);

    switch (role) {
        case RECEIVER:
            setStateMachineRole(RECEIVER);
            if (recv_init(fd) < 0){
                perror("Could not start RECEIVER");
                return -1;
            }
            break;
        case TRANSMITTER:
            setStateMachineRole(TRANSMITTER);
            if (trans_init(fd) < 0){
                perror("Could not start TRANSMITTER");
                return -1;
            }
            break;
        default:
            return -1;
    }

    return fd;
}

int recv_init(int fd) {
    char message[5];
    readMessage(fd, message, COMMAND_SET);
    return sendSupervivionMessage(fd, MSG_A_RECV_RESPONSE, MSG_CTRL_UA, NO_RESPONSE);
}

int trans_init(int fd) {
    return sendSupervivionMessage(fd, MSG_A_TRANS_COMMAND, MSG_CTRL_SET, RESPONSE_UA);
}

int llwrite(int fd, char * buffer, int lenght) {
    char stuffedData[lenght * 2];
    int msgSize = messageStuffing(buffer, lenght, stuffedData);

    return sendDataMessage(fd, stuffedData, msgSize);
}

int llread(int fd, char * buffer) {
    char stuffedMessage[255]; // MAX MESSAGE SIZE
    int numBytesRead = readMessage(fd, stuffedMessage, COMMAND_DATA);
    int res = messageDestuffing(stuffedMessage, 4, numBytesRead - 1, buffer);
    sendSupervivionMessage(fd, MSG_A_RECV_RESPONSE, MSG_CTRL_RR0, NO_RESPONSE);
    return res;
}

int llclose(int fd) {
    switch (getRole()) {
        case RECEIVER:
            if (recv_disc(fd) < 0){
                perror("Could not disconnect RECEIVER");
                return -1;
            }
            break;
        case TRANSMITTER:
            if (trans_disc(fd) < 0){
                perror("Could not disconnect TRANSMITTER");
                return -1;
            }
            break;
        default:
            return -1;
    }

    if (tcsetattr(fd, TCSANOW, &oldtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }
 
    return close(fd);
}

int recv_disc(int fd) {
    printf("DISCONNECTING RECEIVER...\n");
    char message[5];
    readMessage(fd, message, COMMAND_DISC);
    return sendSupervivionMessage(fd, MSG_A_RECV_COMMAND, MSG_CTRL_DISC, RESPONSE_UA);
}

int trans_disc(int fd) {
    printf("DISCONNECTING TRANSMITTER...\n");
    sendSupervivionMessage(fd, MSG_A_TRANS_COMMAND, MSG_CTRL_DISC, COMMAND_DISC);
    return sendSupervivionMessage(fd, MSG_A_TRANS_RESPONSE, MSG_CTRL_UA, NO_RESPONSE);
}