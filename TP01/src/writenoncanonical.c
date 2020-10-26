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
 
    int fd;
    if ((fd = llopen(port, TRANSMITTER)) < 0) {
      perror("llopen failed");
      return 1;
    }
 

    char test[] = "~ ola (}";
    char stuffedMsg[18];
    int msgSize = messageStuffing(test, 9, stuffedMsg);
    write(fd, stuffedMsg, msgSize);
    printf("Message Sent!\n");
 
    sleep(1);
   
    if (llclose(fd) < 0) {
      perror("llclose failed");
      return 1;
    }

    return 0;
}