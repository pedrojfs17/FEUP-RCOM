#include "message.h"

int retry = FALSE;

void atende() {
    retry = TRUE;
}

int sendSET(int fd) {
    char msg[5] = {
        MSG_FLAG, 
        MSG_A_TRANS_COMMAND, 
        MSG_CTRL_SET, 
        MSG_A_TRANS_COMMAND ^ MSG_CTRL_SET, 
        MSG_FLAG
    };

    if (sendMessageWithResponse(fd, msg, MSG_SET_SIZE, RESPONSE_UA) < 0)
        return -1;
    
    return 0;
}

int sendUA_TRANS(int fd) {
    char msg[MSG_SET_SIZE] = {
        MSG_FLAG, 
        MSG_A_TRANS_RESPONSE, 
        MSG_CTRL_UA, 
        MSG_A_TRANS_RESPONSE ^ MSG_CTRL_UA, 
        MSG_FLAG
    };
    
    if (sendMessageWithoutResponse(fd, msg, MSG_SET_SIZE) < 0)
        return -1;
    
    return 0;
}

int sendUA_RECV(int fd) {
    char msg[MSG_SET_SIZE] = {
        MSG_FLAG, 
        MSG_A_RECV_RESPONSE, 
        MSG_CTRL_UA, 
        MSG_A_RECV_RESPONSE ^ MSG_CTRL_UA, 
        MSG_FLAG
    };
    
    if (sendMessageWithoutResponse(fd, msg, MSG_SET_SIZE) < 0)
        return -1;
    
    return 0;
}

int sendDISC_TRANS(int fd) {
    char msg[MSG_SET_SIZE] = {
        MSG_FLAG, 
        MSG_A_TRANS_COMMAND, 
        MSG_CTRL_DISC, 
        MSG_A_TRANS_COMMAND ^ MSG_CTRL_DISC, 
        MSG_FLAG
    };

    if (sendMessageWithResponse(fd, msg, MSG_SET_SIZE, COMMAND_DISC) < 0)
        return -1;
    
    return 0;
}

int sendDISC_RECV(int fd) {
    char msg[5] = {
        MSG_FLAG, 
        MSG_A_RECV_COMMAND, 
        MSG_CTRL_DISC, 
        MSG_A_RECV_COMMAND ^ MSG_CTRL_DISC, 
        MSG_FLAG
    };

    if (sendMessageWithResponse(fd, msg, MSG_SET_SIZE, RESPONSE_UA) < 0)
        return -1;
    
    return 0;
}

int sendMessageWithResponse(int fd, char * msg, int messageSize, mode responseType) {
    configStateMachine(responseType);

    int numTries = 0;

    do {
        numTries++;
        retry = FALSE;

        if (write(fd, msg, messageSize) == -1) {
            perror("WRITE FAILURE!\n");
        }

        alarm(3);

        int res;
        char buf[255];
        while (getState() != STOP && !retry) {
            res = read(fd, buf, 1);
            if (res == 0) continue;
            printf("Byte: %#4.2x\n", buf[0]);
            updateState(buf[0]);
        }

    } while (numTries < 3 && getState() != STOP);

    alarm(0);

    return 0;
}

int sendMessageWithoutResponse(int fd, char * msg, int messageSize) {
    if (write(fd, msg, messageSize) == -1) {
        perror("WRITE FAILURE!\n");
    }

    return 0;
}

void readMessage(int fd, mode responseType) {
    configStateMachine(responseType);
    int res;
    char buf[255];

    while (getState() != STOP) {
        res = read(fd, buf, 1);
        if (res == 0) continue;
        printf("Byte: %#4.2x\n", buf[0]);
        updateState(buf[0]);
    }
}