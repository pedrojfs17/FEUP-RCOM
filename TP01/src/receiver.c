#include "receiver.h"

int receiverApplication(int fd, char* path) {
    char buf[MAX_PACKET_SIZE];
    int res;
    int nump = 0;

    while (1) {
        res = llread(fd, buf);
        nump++;

        // TODO add alarm for timeout

        if (parsePacket(buf, res, path) == END_PACKET) 
            break;

        memset(buf, 0, MAX_PACKET_SIZE); // Resets buffer
    }

    printf("Received %d packets\n", nump);
    return 0;
}

int parsePacket(char * buffer, int lenght, char* path) {
    static int destinationFile;

    if (buffer[0] == START_PACKET) {
        parseControlPacket(buffer, lenght, path);
        destinationFile = open(path, O_RDWR | O_CREAT, 0777);
        return 0;
    }
    else if (buffer[0] == END_PACKET) {
        close(destinationFile);
        return END_PACKET;
    }
    else if (buffer[0] == DATA_PACKET) {
        unsigned dataSize = (unsigned char) buffer[3] + 256 * ((unsigned char) buffer[2]);
        write(destinationFile, &buffer[4], dataSize);
        return 0;
    }
    else {
        printf("Failed on: '");
        for (int i = 0; i < lenght; i++) {
            printf("0x%02x ", buffer[i]);
        }
        printf("'\n");
        return -1;
    }
}

void parseControlPacket(char * buffer, int lenght, char* path) {
    // unsigned fileSize = 0;

    for (int i = 1; i < lenght; i++) {
        if (buffer[i] == FILE_SIZE) {
            i++; // i is now in the byte with information about the number of bytes
            // for (int j = 0; j < buffer[i]; j++) {
            //     fileSize |= (buffer[i+j+1] << (8*j));
            // }
            i += buffer[i];
            // app->fileSize = fileSize;
        }

        if (buffer[i] == FILE_NAME) {
            i++; // i is now in the byte with information about the number of bytes
            strcat(path, &buffer[i+1]);
            i += buffer[i];
        }
    }
}