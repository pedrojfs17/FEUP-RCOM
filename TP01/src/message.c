#include "message.h"

int alarm_flag = FALSE;

void alarm_handler() {
    alarm_flag = TRUE;
}

int sendSupervisionMessage(int fd, unsigned char address, unsigned char control, mode responseType) {
    unsigned char msg[5] = {
        MSG_FLAG, 
        address, 
        control, 
        BCC(address, control), 
        MSG_FLAG
    };

    if (responseType != NO_RESPONSE) {
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

int sendDataMessage(int fd, unsigned char * data, int dataSize, int packet) {
    int msgSize = dataSize + 5;

    unsigned char msg[msgSize];

    msg[0] = MSG_FLAG;
    msg[1] = MSG_A_TRANS_COMMAND;
    msg[2] = MSG_CTRL_S(packet);
    msg[3] = BCC(MSG_A_TRANS_COMMAND, MSG_CTRL_S(packet));
    unsigned char bcc2 = data[0];
    for (int i = 0; i < dataSize; i++) {
        msg[i + 4] = data[i];
        if (i > 0) bcc2 ^= data[i];
    }
    msg[dataSize + 4] = bcc2;

    unsigned char stuffedData[msgSize * 2];
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

        if (ret > 0 && ((packet == 0 && response == R_RR1) || (packet == 1 && response == R_RR0))) {
            receivedACK = TRUE;
        } else if (ret > 0) {
            fprintf(stderr, "Received response is invalid. Trying again...\n");
        }
    } while (numTries < 3 && !receivedACK);

    if (!receivedACK) {
        fprintf(stderr, "Failed to get ACK\n");
        return -1;
    }
    else
        return ret;
}

int writeMessage(int fd, unsigned char * msg, int messageSize) {
    static int wrongBcc1 = 0;
    static int wrongBcc2 = 0;

    int ret;

    if (messageWithError(BCC1_ERROR_PERCENTAGE)) {
        wrongBcc1++;
        fprintf(stderr, "Number of BCC1 errors: %d\n", wrongBcc1);
        unsigned char msgWithError[MAX_BUFFER_SIZE];
        memcpy(msgWithError, msg, messageSize);
        msgWithError[2] ^= 0xFF;
        if ((ret = write(fd, msgWithError, messageSize)) == -1) {
            fprintf(stderr, "Write failed\n");
        }
    }
    else if (messageWithError(BCC2_ERROR_PERCENTAGE)) {
        wrongBcc2++;
        fprintf(stderr, "Number of BCC2 errors: %d\n", wrongBcc2);
        unsigned char msgWithError[MAX_BUFFER_SIZE];
        memcpy(msgWithError, msg, messageSize);
        msgWithError[messageSize - 2] ^= 0xFF;
        if ((ret = write(fd, msgWithError, messageSize)) == -1) {
            fprintf(stderr, "Write failed\n");
        }
    }
    else {
        if ((ret = write(fd, msg, messageSize)) == -1) {
            fprintf(stderr, "Write failed\n");
        }
    }

    return ret;
}

int sendMessageWithResponse(int fd, unsigned char * msg, int messageSize, mode responseType) {
    configStateMachine(responseType);

    int numTries = 0;
    int ret;

    do {
        numTries++;
        alarm_flag = FALSE;

        ret = writeMessage(fd, msg, messageSize);

        alarm(3);

        int res;
        unsigned char buf[MAX_BUFFER_SIZE];
        while (getState() != STOP && !alarm_flag) {
            res = read(fd, buf, 1);
            if (res == 0) continue;
            updateState(buf[0]);
        }

    } while (numTries < 3 && getState() != STOP);

    alarm(0);

    if (getState() != STOP) {
        fprintf(stderr, "Failed to get response!\n");
        return -1;
    }

    usleep(T_PROP);

    return ret;
}

int sendMessageWithoutResponse(int fd, unsigned char * msg, int messageSize) {
    if (write(fd, msg, messageSize) == -1) {
        fprintf(stderr, "Write failed\n");
    }
    return 0;
}

int readMessage(int fd, unsigned char * message, mode responseType) {
    configStateMachine(responseType);
    int res, numBytesRead = 0;
    unsigned char buf[MAX_BUFFER_SIZE];
    alarm_flag = FALSE;

    alarm(7);
    
    while (getState() != STOP && !alarm_flag && numBytesRead < MAX_BUFFER_SIZE) {
        res = read(fd, buf, 1);
        if (res == 0) continue;
        alarm(0);
        message[numBytesRead++] = buf[0];
        updateState(buf[0]);
        alarm(7);
    }

    alarm(0);

    if (alarm_flag) {
        fprintf(stderr, "Alarm fired. readMessage took too long\n");
        return -1;
    }

    if (getState() != STOP) {
        fprintf(stderr, "Failed to read message\n");
        return -1;
    }

    usleep(T_PROP);

    return numBytesRead;
}