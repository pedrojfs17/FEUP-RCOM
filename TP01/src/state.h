#pragma once

#include <stdio.h>

#include "message_defs.h"

#define BCC(addr, ctrl) (addr^ctrl)

typedef enum {START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STOP} msg_state;
typedef enum {RESPONSE_UA, RESPONSE_RR_REJ, COMMAND_SET, COMMAND_DISC, COMMAND_DATA} mode;

typedef struct {
    msg_state currentState;
    mode currentMode;
    int role;
    char control;
    char address;
} stateMachine;

msg_state getState();

int getRole();

void setStateMachineRole(int role);

void configStateMachine(mode stateMachineMode);

void resetState();

void updateState(char byte);

void FlagRCV_stateHandler(char byte);

void ARCV_stateHandler(char byte);

void CRCV_stateHandler(char byte);