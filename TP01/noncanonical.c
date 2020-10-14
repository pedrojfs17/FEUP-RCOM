/*Non-Canonical Input Processing*/
 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
 
#include "message_defs.h"

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
 
//volatile int STOP=FALSE;
 
int send_UA(int serial_fd) {
  char msg[MSG_SET_SIZE] = {MSG_FLAG, MSG_A_REC, MSG_CTRL_UA, MSG_A_REC^MSG_CTRL_UA, MSG_FLAG};
  return write(serial_fd, msg, MSG_SET_SIZE);
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
    else if (byte == MSG_A_EMT)
      *state = A_RCV;
    else
      *state = START;
    break;
  case A_RCV:
    if (byte == MSG_FLAG)
      break;
    else if (byte == MSG_CTRL_SET)
      *state = C_RCV;
    else
      *state = START;
    break;
  case C_RCV:
    if (byte == MSG_FLAG)
      break;
    else if (byte == MSG_A_EMT ^ MSG_CTRL_SET)
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
 
    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */
 
 
 
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
 
    State state = START;
 
    while (state != STOP) {       /* loop for input */
      res = read(fd,buf,1);   /* returns after 1 char has been input */
      printf("byte: %#4.2x\n", buf[0]);
      updateState(&state, buf[0]);
    }

    if (send_UA(fd) == -1) {
      perror("UA FAILURE");
    }

    printf("Going to read message: ");
    //fflush(stdout);

    //char msg[255];
    int numBytesRead = 0;

    /*while (buf[0] != '\0') {
      printf("Reading byte");
      res = read(fd, buf, 1);
      //msg[numBytesRead] = buf[0];
      printf("%s", buf[0]);
      numBytesRead += res;
    }*/
 
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