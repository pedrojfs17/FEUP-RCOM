#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
 
#include "utils.h"
#include "state.h"

#define FALSE 0
#define TRUE 1

#define NO_RESPONSE -1

void atende();

int sendSupervivionMessage(int fd, char address, char control, mode responseType);

int sendDataMessage(int fd, char * data, int dataSize, char bcc2, int packet);

int sendMessageWithResponse(int fd, char * msg, int messageSize, mode responseType);

int sendMessageWithoutResponse(int fd, char * msg, int messageSize);

int readMessage(int fd, char * message, mode responseType);