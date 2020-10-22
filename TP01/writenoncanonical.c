/*Non-Canonical Input Processing*/
 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "ll.h"
 
#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

extern struct termios oldtio;

void send_byte(int serial_fd, char byte) {
  write(serial_fd, &byte, sizeof(char));
  printf("Sent byte: %#4.2x\n", byte);
}

void parseByte(int serial_fd, char byte) {
  if (byte == MSG_FLAG || byte == ESCAPE) {
    send_byte(serial_fd, 0x7d);
    send_byte(serial_fd, byte ^ 0x20);
  }
  else {
    send_byte(serial_fd, byte);
  }
}


int main(int argc, char** argv)
{
    if ( (argc < 2) || 
         ((strcmp("/dev/ttyS0", argv[1])!=0) && 
          (strcmp("/dev/ttyS1", argv[1])!=0) &&
          (strcmp("/dev/ttyS10", argv[1])!=0) &&
          (strcmp("/dev/ttyS11", argv[1])!=0))) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }
 
    int port;

    if (strcmp("/dev/ttyS0", argv[1]) == 0) {
      port = COM0;
    } else if (strcmp("/dev/ttyS1", argv[1]) == 0) {
      port = COM1;
    } else if (strcmp("/dev/ttyS10", argv[1]) == 0) {
      port = COM10;
    } else if (strcmp("/dev/ttyS11", argv[1]) == 0) {
      port = COM11;
    }

  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
 
    int fd = llopen(port, TRANSMITTER);
 

    char test[] = "~ ola (}";

    for (int i = 0; i < sizeof(test); i++) {
      parseByte(fd, test[i]);
    } 
 
    sleep(1);
   
    if (tcsetattr(fd,TCSANOW, &oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }
 
    close(fd);
    return 0;
}