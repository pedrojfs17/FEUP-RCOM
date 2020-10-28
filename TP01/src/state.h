#pragma once

#include <stdio.h>

#include "message_defs.h"

typedef enum {START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, WAITING_DATA, STOP} msg_state;
typedef enum {RESPONSE_UA, RESPONSE_RR_REJ, COMMAND_SET, COMMAND_DISC, COMMAND_DATA} mode;

typedef struct {
    msg_state currentState;
    mode currentMode;
    int role;
    unsigned char control;
    unsigned char address;
} stateMachine;

msg_state getState();

int getRole();

void setStateMachineRole(int role);

void configStateMachine(mode stateMachineMode);

void resetState();

void updateState(unsigned char byte);

void FlagRCV_stateHandler(unsigned char byte);

void ARCV_stateHandler(unsigned char byte);

void CRCV_stateHandler(unsigned char byte);

void WaitingData_stateHandler(unsigned char byte);