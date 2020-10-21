/*Non-Canonical Input Processing*/
 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "message_defs.h"
 
#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

int numTries = 0;
int retry = FALSE;

void atende() {
  retry = TRUE;
}

int send_SET(int serial_fd) {
  char msg[MSG_SET_SIZE] = {MSG_FLAG, MSG_A_EMT, MSG_CTRL_SET, MSG_A_EMT^MSG_CTRL_SET, MSG_FLAG};
  return write(serial_fd, msg, MSG_SET_SIZE);
}

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

void updateState(State * state, char byte) {
  switch (*state)
  {
  case START:
    if (byte == MSG_FLAG)
      *state = FLAG_RCV;
    break;
  case FLAG_RCV:
    if (byte == MSG_FLAG)
      break;
    else if (byte == MSG_A_REC)
      *state = A_RCV;
    else
      *state = START;
    break;
  case A_RCV:
    if (byte == MSG_FLAG)
      break;
    else if (byte == MSG_CTRL_UA)
      *state = C_RCV;
    else
      *state = START;
    break;
  case C_RCV:
    if (byte == MSG_FLAG)
      break;
    else if (byte == MSG_A_REC ^ MSG_CTRL_UA)
      *state = BCC_OK;
    else
      *state = START;
    break;
  case BCC_OK:
    if (byte == MSG_FLAG)
      *state = STOP;
    else
      *state = START;
    break;
  case STOP:
    break;
  }
}
 
int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];
    int i, sum = 0, speed = 0;
    
    if ( (argc < 2) || 
         ((strcmp("/dev/ttyS0", argv[1])!=0) && 
          (strcmp("/dev/ttyS1", argv[1])!=0) &&
          (strcmp("/dev/ttyS10", argv[1])!=0) &&
          (strcmp("/dev/ttyS11", argv[1])!=0))) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }
 
 
  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
 
 
    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }
 
    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }
 
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
 
    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;
 
    newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */
 
 
 
  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) próximo(s) caracter(es)
  */
 
 
 
    tcflush(fd, TCIOFLUSH);
 
    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }
 
    printf("New termios structure set\n");

    (void) signal(SIGALRM, atende);

    State state = START;

    do {
      numTries++;
      retry = FALSE;

      if (send_SET(fd) == -1) {
        perror("SET FAILURE");
      }

      alarm(3);

      while (state != STOP && !retry) {       /* loop for input */
        res = read(fd,buf,1);   /* returns after 1 char has been input */
        if (res == 0) continue;
        printf("UA byte: %#4.2x\n", buf[0]);
        updateState(&state, buf[0]);
      }

    } while (numTries < 3 && state != STOP);

    char test[] = "~ ola (}";

    for (int i = 0; i < sizeof(test); i++) {
      parseByte(fd, test[i]);
    }


    //printf("Enter a string: ");
    //gets(buf);
 
    //printf("You entered: %s", buf);
    
    //res = write(fd,buf,strlen(buf)+1);   
    //printf("%d bytes written\n", res);

    //while (STOP==FALSE) {       /* loop for input */
      //res = read(fd,buf,255);   /* returns after 5 chars have been input */
      //buf[res]=0;               /* so we can printf... */
      //printf("%s", buf);
      //if (buf[res-1]=='\0') STOP=TRUE;
    //}
    //printf("\n");
 
 
  /* 
    O ciclo FOR e as instruções seguintes devem ser alterados de modo a respeitar 
    o indicado no guião 
  */
 
 
    sleep(1);
   
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }
 
    close(fd);
    return 0;
}