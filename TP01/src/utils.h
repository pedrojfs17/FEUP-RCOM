#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "message_defs.h"

int messageStuffing(char * buffer, int startingByte, int lenght, char * stuffedMessage);

int messageDestuffing(char * buffer, int startingByte, int lenght, char * destuffedMessage);

char BCC2(char * data, int dataSize, int startingByte);