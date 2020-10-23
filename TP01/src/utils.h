#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "message_defs.h"

int messageStuffing(char * buffer, int lenght, char * stuffedMessage);

int messageDestuffing(char * buffer, int lenght, char * destuffedMessage);
