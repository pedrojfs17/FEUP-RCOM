#include "receiver.h"

int receiverApplication(int fd, char* path) {
    int res;
    int nump = 0;
    int numTries = 0;

    while (1) {
        unsigned char buf[MAX_PACKET_SIZE];
        if ((res = llread(fd, buf)) < 0) {
            if (numTries > 9) return -1;
            numTries++;
            continue;
        }

        if (numTries > 0)
            fprintf(stderr, "It took %d tries to read!\n", numTries);

        numTries = 0;
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

int parsePacket(unsigned char * buffer, int lenght, char* path) {
    static int destinationFile;
    static int filesize = 0;

    if (buffer[0] == START_PACKET) {
        parseControlPacket(buffer, lenght, path, &filesize);
        
        if ((destinationFile = open(path, O_WRONLY | O_CREAT, 0777)) < 0) {
            perror("Error opening destination file!");
            return -1;
        }

        return 0;
    } else if (buffer[0] == END_PACKET) {
        if (close(destinationFile) < 0) {
            perror("Error closing destination file!");
            return -1;
        }

        if (checkFileSize(path, filesize) != 0) {
            fprintf(stderr, "Recieved file size different from expected\n");
            return -1; 
        }

        return END_PACKET;
    } else if (buffer[0] == DATA_PACKET) {
        if (checkSequenceNumber(buffer[1]) != 0) {
            fprintf(stderr, "Error, invalid sequence number\n");
            return -1;
        }
        unsigned dataSize = buffer[3] + 256 * buffer[2];
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

void parseControlPacket(unsigned char * buffer, int lenght, char* path, int* filesize) {
    for (int i = 1; i < lenght; i++) {
        if (buffer[i] == FILE_SIZE) {
            i++; // i is now in the byte with information about the number of bytes
            for (int j = 0; j < buffer[i]; j++) {
                *filesize |= (buffer[i+j+1] << (8*j));
            }
            i += buffer[i];
        }

        if (buffer[i] == FILE_NAME) {
            i++; // i is now in the byte with information about the number of bytes
            char fileName[buffer[i] + 1];
            for (int j = 0; j < buffer[i]; j++) {
                fileName[j] = buffer[i + j + 1];
            }
            fileName[buffer[i]] = '\0';
            strcat(path, fileName);
            i += buffer[i];
        }
    }
}

int checkFileSize(char* path, int filesize) {
    struct stat file_stat;

    // Reads file info using stat
    if (stat(path, &file_stat)<0){
        perror("Error getting file information.");
        return -1;
    }

    return (filesize != file_stat.st_size);
}

int checkSequenceNumber(unsigned r_sn) {
    static unsigned sequence_num = 0;

    if (r_sn == sequence_num) {
        sequence_num = (sequence_num + 1) % 255;
        return 0;
    }

    return -1;
}