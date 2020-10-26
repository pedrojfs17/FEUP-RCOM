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

void atende();

int sendSET(int fd);

int sendUA_TRANS(int fd);

int sendUA_RECV(int fd);

int sendDISC_TRANS(int fd);

int sendDISC_RECV(int fd);

int sendMessageWithResponse(int fd, char * msg, int messageSize, mode responseType);

int sendMessageWithoutResponse(int fd, char * msg, int messageSize);

void readMessage(int fd, mode responseType);
