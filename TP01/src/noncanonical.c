/*Non-Canonical Input Processing*/
 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
 
#include "ll.h"

#define BAUDRATE B38400
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
    if ((fd = llopen(port, RECEIVER)) < 0) {
      perror("llopen failed");
      return 1;
    }

    printf("Connected\n");

    char buf[255];
    int res;

    res = llread(fd, buf);

    printf("Read %d bytes. Message: '", res);
    for (int i = 0; i < res; i++) {
        printf("%c", buf[i]);
    }
    printf("'\n");

  /*
    O ciclo WHILE deve ser alterado de modo a respeitar o indicado no guião 
  */
 
    sleep(1);
    
    if (llclose(fd) < 0) {
      perror("llclose failed");
      return 1;
    }

    return 0;
}