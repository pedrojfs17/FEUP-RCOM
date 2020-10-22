#pragma once

#include <stdio.h>

#include "message_defs.h"

#define BCC(addr, ctrl) (addr^ctrl)

typedef enum {START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STOP} msg_state;

msg_state getState();

void resetState();

void SET_UA_updateState(char byte);