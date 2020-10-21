/* Application layer functions */

#include "ll.h"

int llopen(int port, int role) {
    char port_str[12];
    sprintf(port_str, "/dev/ttyS%d", port);

    int fd = open(port_str, O_RDWR | O_NOCTTY);
    if (fd < 0) { 
        perror(port_str);
        return -1;
    }

    struct termios oldtio, newtio;

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
 
    newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */
 
    tcflush(fd, TCIOFLUSH);
 
    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
        perror("tcsetattr");
        return -1;
    }
 
    printf("New termios structure set\n");

    switch (role) {
        case RECEIVER:
            if (recv_init(fd) != 0){
                perror("Could not start RECEIVER");
                return -1;
            }
            break;
        case TRANSMITTER:
            if (trans_init(fd) != 0){
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
    return 0;
}

int trans_init(int fd) {
    return 0;
}