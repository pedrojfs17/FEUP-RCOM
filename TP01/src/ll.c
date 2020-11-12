/* Application layer functions */

#include "ll.h"

struct termios oldtio;

int llopen(int port, int role) {
    char portString[15];
    sprintf(portString, "/dev/ttyS%d", port);
    int fd = open(portString, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("Error opening serial port");
        return -1;
    }

    struct termios newtio;

    if (tcgetattr(fd, &oldtio) == -1) { /* save current port settings */
        perror("tcgetattr failed");
        return -1;
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
 
    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;
 
    newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */
 
    tcflush(fd, TCIOFLUSH);
 
    if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
        perror("tcsetattr failed to set new termios struct");
        return -1;
    }
 
    printf("New termios structure set\n");

    signal(SIGALRM, alarm_handler);

    switch (role) {
        case RECEIVER:
            setStateMachineRole(RECEIVER);
            if (recv_init(fd) < 0){
                fprintf(stderr, "Could not start RECEIVER\n");
                return -1;
            }
            break;
        case TRANSMITTER:
            setStateMachineRole(TRANSMITTER);
            if (trans_init(fd) < 0){
                fprintf(stderr, "Could not start TRANSMITTER\n");
                return -1;
            }
            break;
        default:
            return -1;
    }

    return fd;
}

int recv_init(int fd) {
    unsigned char message[5];
    if (readMessage(fd, message, COMMAND_SET) < 0) return -1;
    return sendSupervisionMessage(fd, MSG_A_RECV_RESPONSE, MSG_CTRL_UA, NO_RESPONSE);
}

int trans_init(int fd) {
    return sendSupervisionMessage(fd, MSG_A_TRANS_COMMAND, MSG_CTRL_SET, RESPONSE_UA);
}

int llwrite(int fd, unsigned char * buffer, int lenght) {
    static int packet = 0;
    
    int ret;
    int numTries = 0;

    while (numTries < 3) {
        numTries++;
        if ((ret = sendDataMessage(fd, buffer, lenght, packet)) > -1) {
            packet = (packet + 1) % 2;
            return ret;
        }
        fprintf(stderr, "sendDataMessage failed\n");
    }

    return -1;
}

int llread(int fd, unsigned char * buffer) {
    static int packet = 0;
    unsigned char stuffedMessage[MAX_BUFFER_SIZE], unstuffedMessage[MAX_PACKET_SIZE + 7]; // MAX MESSAGE SIZE
    int numBytesRead;
    if ((numBytesRead = readMessage(fd, stuffedMessage, COMMAND_DATA)) < 0) {
        fprintf(stderr, "Read operation failed\n");
        return -1;
    }
    int res = messageDestuffing(stuffedMessage, 1, numBytesRead - 1, unstuffedMessage);
    
    unsigned char receivedBCC2 = unstuffedMessage[res - 1];
    unsigned char receivedDataBCC2 = BCC2(unstuffedMessage, res - 1, 4);

    if (receivedBCC2 == receivedDataBCC2 && unstuffedMessage[2] == MSG_CTRL_S(packet)) {
        packet = (packet + 1) % 2;
        if (sendSupervisionMessage(fd, MSG_A_RECV_RESPONSE, MSG_CTRL_RR(packet), NO_RESPONSE) < 0) return -1;
        memcpy(buffer, &unstuffedMessage[4], res-5);
        return res - 5;
    }
    else if (receivedBCC2 == receivedDataBCC2) {
        sendSupervisionMessage(fd, MSG_A_RECV_RESPONSE, MSG_CTRL_RR(packet), NO_RESPONSE);
        fprintf(stderr, "Duplicate Packet!\n");
        tcflush(fd, TCIFLUSH);
        return -1;
    } else {
        sendSupervisionMessage(fd, MSG_A_RECV_RESPONSE, MSG_CTRL_REJ(packet), NO_RESPONSE);
        fprintf(stderr, "Error in BCC2, sent REJ!\n");
        tcflush(fd, TCIFLUSH);
        return -1;
    }
}

int llclose(int fd) {
    switch (getRole()) {
        case RECEIVER:
            if (recv_disc(fd) < 0){
                fprintf(stderr, "Could not disconnect RECEIVER\n");
                return -1;
            }
            break;
        case TRANSMITTER:
            if (trans_disc(fd) < 0){
                fprintf(stderr, "Could not disconnect TRANSMITTER\n");
                return -1;
            }
            break;
        default:
            return -1;
    }

    sleep(1);

    if (tcsetattr(fd, TCSANOW, &oldtio) == -1) {
        perror("tcsetattr failed to set old termios struct");
        exit(-1);
    }
 
    return close(fd);
}

int recv_disc(int fd) {
    printf("DISCONNECTING RECEIVER...\n");
    unsigned char message[5];
    if (readMessage(fd, message, COMMAND_DISC) < 0) return -1;
    return sendSupervisionMessage(fd, MSG_A_RECV_COMMAND, MSG_CTRL_DISC, RESPONSE_UA);
}

int trans_disc(int fd) {
    printf("DISCONNECTING TRANSMITTER...\n");
    if (sendSupervisionMessage(fd, MSG_A_TRANS_COMMAND, MSG_CTRL_DISC, COMMAND_DISC) < 0) return -1;
    return sendSupervisionMessage(fd, MSG_A_TRANS_RESPONSE, MSG_CTRL_UA, NO_RESPONSE);
}