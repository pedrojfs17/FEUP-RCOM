#include "message.h"

int retry = FALSE;

void atende() {
    retry = TRUE;
}

int sendSupervivionMessage(int fd, char address, char control, mode responseType) {
    printf("CONTROL = %#4.2x\n", control);
    char msg[5] = {
        MSG_FLAG, 
        address, 
        control, 
        BCC(address, control), 
        MSG_FLAG
    };

    if (responseType != -1) {
        if (sendMessageWithResponse(fd, msg, MSG_SET_SIZE, responseType) < 0)
            return -1;

        return 0;
    }
    else {
        if (sendMessageWithoutResponse(fd, msg, MSG_SET_SIZE) < 0)
            return -1;
    
        return 0;
    }
}

int sendDataMessage(int fd, char * data, int dataSize, char bcc2, int packet) {
    int msgSize = dataSize + 6;

    char msg[msgSize];

    msg[0] = MSG_FLAG;
    msg[1] = MSG_A_TRANS_COMMAND;
    msg[2] = MSG_CTRL_S(packet);
    msg[3] = BCC(MSG_A_TRANS_COMMAND, MSG_CTRL_S(packet));
    for (int i = 0; i < dataSize; i++) {
        msg[i + 4] = data[i];
    }
    msg[dataSize + 4] = bcc2;
    msg[dataSize + 5] = MSG_FLAG;

    int numTries = 0;
    int receivedACK = FALSE;

    do {
        numTries++;
        sendMessageWithResponse(fd, msg, msgSize, RESPONSE_RR_REJ);

        // Parse response
        receivedACK = TRUE;
    } while (numTries < 3 && !receivedACK);

    if (!receivedACK)
        return -1;
    else
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

int readMessage(int fd, char * message, mode responseType) {
    configStateMachine(responseType);
    int res, numBytesRead = 0;
    char buf[255];

    while (getState() != STOP) {
        res = read(fd, buf, 1);
        if (res == 0) continue;
        printf("Byte: %#4.2x\n", buf[0]);
        message[numBytesRead++] = buf[0];
        updateState(buf[0]);
    }

    return numBytesRead;
}