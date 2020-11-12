/* Application layer functions */

#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
 
#include "utils.h"
#include "state.h"
#include "message.h"

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */

int llopen(int port, int role);
int recv_init(int fd);
int trans_init(int fd);

int llclose(int fd);
int recv_disc(int fd);
int trans_disc(int fd);

int llwrite(int fd, unsigned char * buffer, int lenght);
int llread(int fd, unsigned char * buffer);
