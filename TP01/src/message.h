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

#define FALSE 0
#define TRUE 1

void alarm_handler();

int sendSupervisionMessage(int fd, unsigned char address, unsigned char control, mode responseType);

int sendDataMessage(int fd, unsigned char * data, int dataSize, int packet);

int sendMessageWithResponse(int fd, unsigned char * msg, int messageSize, mode responseType);

int sendMessageWithoutResponse(int fd, unsigned char * msg, int messageSize);

int readMessage(int fd, unsigned char * message, mode responseType);