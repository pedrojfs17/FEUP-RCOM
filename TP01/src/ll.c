/* Application layer functions */

#include "ll.h"

struct termios oldtio;

int retry = FALSE;

void atende() {
    retry = TRUE;
}

int llopen(int port, int role) {
    char port_str[12];
    sprintf(port_str, "/dev/ttyS%d", port);

    int fd = open(port_str, O_RDWR | O_NOCTTY);
    if (fd < 0) { 
        perror(port_str);
        return -1;
    }

    struct termios newtio;

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
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
 
    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
        perror("tcsetattr");
        return -1;
    }
 
    printf("New termios structure set\n");

    switch (role) {
        case RECEIVER:
            if (recv_init(fd) < 0){
                perror("Could not start RECEIVER");
                return -1;
            }
            break;
        case TRANSMITTER:
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
    resetState();

    int res;
    char buf[255];
 
    while (getState() != STOP) {        /* loop for input */
        res = read(fd,buf,1);           /* returns after 1 char has been input */
        printf("Current state: %d\tSET byte: %#4.2x\n", getState(), buf[0]);
        SET_UA_updateState(buf[0]);
    }

    // TODO add retry
    char msg[MSG_SET_SIZE] = {MSG_FLAG, MSG_A_REC, MSG_CTRL_UA, MSG_A_REC^MSG_CTRL_UA, MSG_FLAG};
    
    return write(fd, msg, MSG_SET_SIZE);
}

int trans_init(int fd) {
    signal(SIGALRM, atende);

    resetState();
    
    int numTries = 0;
    do {
        numTries++;
        retry = FALSE;

        char msg[MSG_SET_SIZE] = {MSG_FLAG, MSG_A_EMT, MSG_CTRL_SET, MSG_A_EMT^MSG_CTRL_SET, MSG_FLAG};

        if (write(fd, msg, MSG_SET_SIZE) == -1) {
            perror("SET FAILURE");
        }

        alarm(3);

        int res;
        char buf[255];
        while (getState() != STOP && !retry) {          /* loop for input */
            res = read(fd, buf, 1);                     /* returns after 1 char has been input */
            if (res == 0) continue;
            printf("UA byte: %#4.2x\n", buf[0]);
            SET_UA_updateState(buf[0]);
        }

    } while (numTries < 3 && getState() != STOP);

    return 0;
}