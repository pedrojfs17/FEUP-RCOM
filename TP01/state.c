#include "state.h"

msg_state current_state;

msg_state getState() {
    return current_state;
}

void resetState() {
    current_state = START;
}

void SET_UA_updateState(char byte) {
    static char addr;
    static char ctrl;
    switch (current_state) {
        case START:
            if (byte == MSG_FLAG)
                current_state = FLAG_RCV;
            break;
        case FLAG_RCV:
            if (byte == MSG_FLAG)
                break;
            else if (byte == MSG_A_REC) {
                current_state = A_RCV;
                addr = byte;
            }
            else
                current_state = START;
            break;
        case A_RCV:
            if (byte == MSG_FLAG)
                break;
            else if (byte == MSG_CTRL_UA || byte == MSG_CTRL_SET) {
                current_state = C_RCV;
                ctrl = byte;
            }
            else
                current_state = START;
            break;
        case C_RCV:
            if (byte == MSG_FLAG)
                break;
            else if (byte == BCC(addr, ctrl))
                current_state = BCC_OK;
            else
                current_state = START;
            break;
        case BCC_OK:
            if (byte == MSG_FLAG){
                printf("FINISHED\n");
                current_state = STOP;
            }
            else
                current_state = START;
            break;
        case STOP:
            break;
    }
}