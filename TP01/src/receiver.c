#include "receiver.h"

int receiverApplication(int fd, char* path) {
    int res;
    int nump = 0;

    while (1) {
        char buf[MAX_PACKET_SIZE];
        if ((res = llread(fd, buf)) < 0) return -1;
        nump++;

        int ret;
        if ((ret = parsePacket(buf, res, path)) == END_PACKET) 
            break;
        else if (ret == -1)
            return -1;
    }

    printf("Received %d packets\n", nump);
    return 0;
}

int parsePacket(char * buffer, int lenght, char* path) {
    static int destinationFile;

    if (buffer[0] == START_PACKET) {
        parseControlPacket(buffer, lenght, path);
        
        if ((destinationFile = open(path, O_RDWR | O_CREAT, 0777)) < 0) {
            perror("Error opening destination file!");
            return -1;
        }
            
        return 0;
    } else if (buffer[0] == END_PACKET) {
        if (close(destinationFile) < 0) {
            perror("Error closing destination file!");
            return -1;
        }
        return END_PACKET;
    } else if (buffer[0] == DATA_PACKET) {
        unsigned dataSize = (unsigned char) buffer[3] + 256 * ((unsigned char) buffer[2]);
        if (write(destinationFile, &buffer[4], dataSize) < 0) {
            perror("Error writing to destination file!");
            return -1;
        }
        return 0;
    } else {
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