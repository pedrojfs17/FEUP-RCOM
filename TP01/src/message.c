#include "message.h"

int retry = FALSE;

void atende() {
    retry = TRUE;
}

int sendSupervivionMessage(int fd, char address, char control, mode responseType) {
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
    int msgSize = dataSize + 5;

    char msg[msgSize];

    msg[0] = MSG_FLAG;
    msg[1] = MSG_A_TRANS_COMMAND;
    msg[2] = MSG_CTRL_S(packet);
    msg[3] = BCC(MSG_A_TRANS_COMMAND, MSG_CTRL_S(packet));
    for (int i = 0; i < dataSize; i++) {
        msg[i + 4] = data[i];
    }
    msg[dataSize + 4] = bcc2;

    char stuffedData[msgSize * 2];
    msgSize = messageStuffing(msg, 1, msgSize, stuffedData);
    stuffedData[msgSize] = MSG_FLAG;
    msgSize++;

    int numTries = 0;
    int receivedACK = FALSE;
    int ret;

    do {
        numTries++;
        ret = sendMessageWithResponse(fd, stuffedData, msgSize, RESPONSE_RR_REJ);

        response_type response = getLastResponse();

        if ((packet == 0 && response == R_RR1) || (packet == 1 && response == 0)) {
            receivedACK = TRUE;
        }
    } while (numTries < 3 && !receivedACK);

    if (!receivedACK)
        return -1;
    else
        return ret;
}

int sendMessageWithResponse(int fd, char * msg, int messageSize, mode responseType) {
    configStateMachine(responseType);

    int numTries = 0;
    int ret;

    do {
        numTries++;
        retry = FALSE;

        if ((ret = write(fd, msg, messageSize)) == -1) {
            perror("WRITE FAILURE!\n");
        }

        alarm(3);

        int res;
        unsigned char buf[255];
        while (getState() != STOP && !retry) {
            res = read(fd, buf, 1);
            if (res == 0) continue;
            // printf("Byte: %#4.2x\n", buf[0]);
            updateState(buf[0]);
        }

    } while (numTries < 3 && getState() != STOP);

    alarm(0);

    if (getState() != STOP)
        return -1;

    return ret;
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
    unsigned char buf[255];

    while (getState() != STOP) {
        res = read(fd, buf, 1);
        if (res == 0) continue;
        // printf("Byte: %#4.2x\n", buf[0]);
        message[numBytesRead++] = buf[0];
        updateState(buf[0]);
    }

    return numBytesRead;
}