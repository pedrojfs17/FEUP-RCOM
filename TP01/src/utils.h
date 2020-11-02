#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "message_defs.h"

#define PROGRESS_BAR_SIZE   30
#define SEPARATOR_CHAR      '#'
#define EMPTY_CHAR          '.'
#define NUM_BACKSPACES      PROGRESS_BAR_SIZE + 9

int messageStuffing(char * buffer, int startingByte, int lenght, char * stuffedMessage);

int messageDestuffing(char * buffer, int startingByte, int lenght, char * destuffedMessage);

char BCC2(char * data, int dataSize, int startingByte);

void clearProgressBar();

void printProgressBar(int progress, int total);

char * getFilename(char * path);