#include "utils.h"

int messageStuffing(unsigned char * buffer, int startingByte, int lenght, unsigned char * stuffedMessage) {
    int messageSize = 0;

    for (int i = 0; i < startingByte; i++)
        stuffedMessage[messageSize++] = buffer[i];

    for (int i = startingByte; i < lenght; i++) {
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

int messageDestuffing(unsigned char * buffer, int startingByte, int lenght, unsigned char * destuffedMessage) {
    int messageSize = 0;

    for (int i = 0; i < startingByte; i++) {
        destuffedMessage[messageSize++] = buffer[i];
    }

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

unsigned char BCC2(unsigned char * data, int dataSize, int startingByte) {
    unsigned char bcc = data[startingByte];

    for(int i = startingByte + 1; i < dataSize; i++)
        bcc ^= data[i];

    return bcc;
}

void clearProgressBar() {
    int i;
    for (i = 0; i < NUM_BACKSPACES; ++i) {
        fprintf(stdout, "\b");
    }
    fflush(stdout);
}

void printProgressBar(int progress, int total) {
    int i, percentage = (int)((((double)progress) / total) * 100);
    int num_separators = (int)((((double)progress) / total) * PROGRESS_BAR_SIZE);;
    fprintf(stdout, "[");
    for (i = 0; i < num_separators; ++i) {
        fprintf(stdout, "%c", SEPARATOR_CHAR);
    }
    for (; i < PROGRESS_BAR_SIZE; ++i) {
        fprintf(stdout, "%c", EMPTY_CHAR);
    }
    fprintf(stdout, "]  %2d%%  ", percentage);
    fflush(stdout);
}

char * getFilename(char * path) {
    char * filename = path, *p;
    for (p = path; *p; p++) {
        if (*p == '/' || *p == '\\' || *p == ':') {
            filename = p;
        }
    }
    return filename;
}

int messageWithError(int percentage) {
    int error = rand() % 100;
    
    if (error < percentage)
        return 1;
    
    return 0;
}