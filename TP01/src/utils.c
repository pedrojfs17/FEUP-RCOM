#include "utils.h"

int messageStuffing(char * buffer, int lenght, char * stuffedMessage) {
    int messageSize = 0;

    for (int i = 0; i < lenght; i++) {
        if (buffer[i] == MSG_FLAG || buffer[i] == ESCAPE) {
            stuffedMessage[messageSize++] = 0x7d;
            stuffedMessage[messageSize++] = buffer[i] ^ 0x20;
        }
        else {
            stuffedMessage[messageSize++] = buffer[i];
        }
    }

    return messageSize;
}

int messageDestuffing(char * buffer, int startingByte, int lenght, char * destuffedMessage) {
    int messageSize = 0;

    for (int i = startingByte; i < lenght; i++) {
        if (buffer[i] == ESCAPE) {
            destuffedMessage[messageSize++] = buffer[i + 1] ^ 0x20;
            i++;
        }
        else {
            destuffedMessage[messageSize++] = buffer[i];
        }
    }

    return messageSize;
}