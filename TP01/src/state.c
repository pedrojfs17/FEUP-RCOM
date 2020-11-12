#include "state.h"

stateMachine state;

msg_state getState() {
    return state.currentState;
}

response_type getLastResponse() {
    return state.last_response;
}

int getRole() {
    return state.role;
}

void setStateMachineRole(int role) {
    state.role = role;
}

void configStateMachine(mode stateMachineMode) {
    resetState();
    state.currentMode = stateMachineMode;
}

void resetState() {
    state.currentState = START;
    state.last_response = R_NULL;
}

void updateState(unsigned char byte) {
    switch (state.currentState) {
        case START:
            if (byte == MSG_FLAG)
                state.currentState = FLAG_RCV;
            break;
        case FLAG_RCV:
            FlagRCV_stateHandler(byte);
            break;
        case A_RCV:
            ARCV_stateHandler(byte);
            break;
        case C_RCV:
            CRCV_stateHandler(byte);
            break;
        case WAITING_DATA:
            WaitingData_stateHandler(byte);
            break;
        case BCC_OK:
            if (byte == MSG_FLAG)
                state.currentState = STOP;
            else
                state.currentState = START;
            break;
        case STOP:
            break;
    }
}

void FlagRCV_stateHandler(unsigned char byte) {
    if (byte == MSG_FLAG)
        return;

    switch (state.currentMode) {
        case RESPONSE_UA:
        case RESPONSE_RR_REJ:
            if ((state.role == TRANSMITTER && byte == MSG_A_RECV_RESPONSE) || (state.role == RECEIVER && byte == MSG_A_TRANS_RESPONSE)) {
                state.currentState = A_RCV;
                state.address = byte;
                return;
            }
            break;
        case COMMAND_SET:
        case COMMAND_DISC:
        case COMMAND_DATA:
            if ((state.role == RECEIVER && byte == MSG_A_TRANS_COMMAND) || (state.role == TRANSMITTER && byte == MSG_A_RECV_COMMAND)) {
                state.currentState = A_RCV;
                state.address = byte;
                return;
            }
            break;
    }

    state.currentState = START;
}

void ARCV_stateHandler(unsigned char byte) {
    if (byte == MSG_FLAG) {
        state.currentState = FLAG_RCV;
        return;
    }
    
    switch (state.currentMode) {
        case RESPONSE_UA:
            if (byte == MSG_CTRL_UA) {
                state.currentState = C_RCV;
                state.control = byte;
                return;
            }
            break;
        case RESPONSE_RR_REJ: 
            if (byte == MSG_CTRL_RR(0) || byte == MSG_CTRL_RR(1) || byte == MSG_CTRL_REJ(0) || byte == MSG_CTRL_REJ(1)) {
                state.currentState = C_RCV;
                state.control = byte;
                switch (byte) {
                    case MSG_CTRL_RR(0):
                        state.last_response = R_RR0;
                        break;
                    case MSG_CTRL_RR(1):
                        state.last_response = R_RR1;
                        break;
                    case MSG_CTRL_REJ(0):
                        state.last_response = R_REJ0;
                        break;
                    case MSG_CTRL_REJ(1):
                        state.last_response = R_REJ1;
                        break;
                }
                return;
            }
            break;
        case COMMAND_SET:
            if (byte == MSG_CTRL_SET) {
                state.currentState = C_RCV;
                state.control = byte;
                return;
            }
            break;
        case COMMAND_DISC:
            if (byte == MSG_CTRL_DISC) {
                state.currentState = C_RCV;
                state.control = byte;
                return;
            }
            break;
        case COMMAND_DATA:
            if (byte == MSG_CTRL_S(0) || byte == MSG_CTRL_S(1)) {
                state.currentState = C_RCV;
                state.control = byte;
                return;
            }
            break;
    }

    state.currentState = START;
}

void CRCV_stateHandler(unsigned char byte) {
    if (byte == MSG_FLAG) {
        state.currentState = FLAG_RCV;
        return;
    }

    if (byte == BCC(state.address, state.control))
        if (state.currentMode == COMMAND_DATA)
            state.currentState = WAITING_DATA;
        else
            state.currentState = BCC_OK;
    else
        state.currentState = START;
}

void WaitingData_stateHandler(unsigned char byte) {
    if (byte == MSG_FLAG) {
        state.currentState = STOP;
        return;
    }
    else return;
}