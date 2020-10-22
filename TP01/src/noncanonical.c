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

void parse_msg(char * msg, int size, char * parsedMsg, int * msgSize) {
  *msgSize = 0;

  for (int i = 0; i < size; i++) {
    if (msg[i] == ESCAPE) {
      parsedMsg[*msgSize] = msg[i + 1] ^ 0x20;
      i++;
    }
    else
      parsedMsg[*msgSize] = msg[i];

    (*msgSize)++;
  }
}

int main(int argc, char** argv)
{
    char buf[255];
    int res;
 
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

    int fd = llopen(port, RECEIVER);

    char msg[255], parsedMsg[256];
    int numBytesRead = 0, msgSize;
    buf[0] = 'a';
    while (buf[0] != '\0') {
      res = read(fd, buf, 1);
      msg[numBytesRead] = buf[0];
      numBytesRead++;
      printf("Received byte: %#4.2x\n", buf[0]);
    }

    parse_msg(msg, numBytesRead, parsedMsg, &msgSize);

    printf("MSG: ");
    for (int i = 0; i < msgSize; i++) {
      printf("%c", parsedMsg[i]);
    }
    printf("\n");
 
    //res = write(fd,buf,strlen(buf)+1);   
    //printf("%d bytes written\n", res);
 
  /* 
    O ciclo WHILE deve ser alterado de modo a respeitar o indicado no guião 
  */
 
 
    sleep(1);
    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}