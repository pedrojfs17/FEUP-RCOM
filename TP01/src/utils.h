#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "message_defs.h"

#define PROGRESS_BAR_SIZE   30
#define SEPARATOR_CHAR      '#'
#define EMPTY_CHAR          '.'
#define NUM_BACKSPACES      PROGRESS_BAR_SIZE + 9

int messageStuffing(unsigned char * buffer, int startingByte, int lenght, unsigned char * stuffedMessage);

int messageDestuffing(unsigned char * buffer, int startingByte, int lenght, unsigned char * destuffedMessage);

unsigned char BCC2(unsigned char * data, int dataSize, int startingByte);

void clearProgressBar();

void printProgressBar(int progress, int total);

char * getFilename(char * path);

int messageWithError(int percentage);
