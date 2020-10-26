/* Application layer functions */

#include "ll.h"

struct termios oldtio;
struct linkLayer ll;

int retry = FALSE;

void atende() {
    retry = TRUE;
}

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
    configStateMachine(COMMAND_SET);

    int res;
    char buf[255];
 
    while (getState() != STOP) {        /* loop for input */
        res = read(fd,buf,1);           /* returns after 1 char has been input */
        printf("Current state: %d\tSET byte: %#4.2x\n", getState(), buf[0]);
        updateState(buf[0]);
    }

    // TODO add retry
    char msg[MSG_SET_SIZE] = {MSG_FLAG, MSG_A_RECV_RESPONSE, MSG_CTRL_UA, MSG_A_RECV_RESPONSE^MSG_CTRL_UA, MSG_FLAG};
    
    return write(fd, msg, MSG_SET_SIZE);
}

int trans_init(int fd) {
    signal(SIGALRM, atende);

    configStateMachine(RESPONSE_UA);
    
    int numTries = 0;
    do {
        numTries++;
        retry = FALSE;

        char msg[MSG_SET_SIZE] = {MSG_FLAG, MSG_A_TRANS_COMMAND, MSG_CTRL_SET, MSG_A_TRANS_COMMAND^MSG_CTRL_SET, MSG_FLAG};

        if (write(fd, msg, MSG_SET_SIZE) == -1) {
            perror("SET FAILURE");
        }

        alarm(3);

        int res;
        char buf[255];
        while (getState() != STOP && !retry) {          /* loop for input */
            res = read(fd, buf, 1);                     /* returns after 1 char has been input */
            if (res == 0) continue;
            printf("Current state: %d\tUA byte: %#4.2x\n", getState(), buf[0]);
            updateState(buf[0]);
        }

    } while (numTries < 3 && getState() != STOP);

    return 0;
}

int llwrite(int fd, char * buffer, int lenght) {
    int totalBytesSent = 0, bytesSent, numTries = 0;
    char stuffedMessage[lenght * 2];

    int messageSize = messageStuffing(buffer, lenght, stuffedMessage);

    do {

        do {
            numTries++;
            retry = FALSE;

            if ((bytesSent = write(fd, stuffedMessage, messageSize)) == -1) {
                perror("Error writing message");
            }

            alarm(3);

            resetState();

            int res;
            char byte;
            while (getState() != STOP && !retry) {          /* loop for input */
                res = read(fd, byte, 1);                     /* returns after 1 char has been input */
                if (res == 0) continue;
                printf("Received response byte: %#4.2x\n", byte);
                updateState(byte);
            }

        } while (numTries < 3);

        totalBytesSent += bytesSent;

    } while (totalBytesSent < lenght);
    
    alarm(0);

    if (totalBytesSent < lenght)
        return -1;

    printf("SENT MESSAGE WITH %d BYTES\n", bytesSent);

    return bytesSent;
}

int llread(int fd, char * buffer) {
    
}

int llclose(int fd) {
    if (tcsetattr(fd, TCSANOW, &oldtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }
 
    close(fd);
}