#include "application.h"

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
        fprintf(stdout, " ");
    }
    fprintf(stdout, "]  %2d%%  ", percentage);
    fflush(stdout);
}

int checkArgs(int argc, char ** argv, applicationArgs * app) {
    if (argc != 5) {
        return -1;
    }

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-t")) {
            if (app->role != -1)
                return -1;
            app->role = TRANSMITTER;
        }
        else if (!strcmp(argv[i], "-r")) {
            if (app->role != -1)
                return -1;
            app->role = RECEIVER;
        }
        else if (!strcmp(argv[i],"-p")){
            if (i + 1 == argc || app->port != -1){
                return -1;
            }
            if (atoi(argv[i+1]) > 0){
                app->port = atoi(argv[i+1]);
                i++;
            }
            else return -1;
        }
        else if (argv[i][0] != '-' && app->path[0] == '\0')
            strcpy(app->path, argv[i]);
        else {
            return -1;
        }
    }

    if (app->path[0] == '\0') return -1;
    if (app->role != TRANSMITTER && app->role != RECEIVER) return -1;
    if (app->port != 0 && app->port != 1 && app->port != 10 && app->port != 11) return -1;

    return 0;
}

int transmitterApplication(int fd, applicationArgs * app) {
    int file_fd;
    struct stat file_stat;

    // Reads file info using stat
    if (stat(app->path, &file_stat)<0){
        perror("Error getting file information.");
        return -1;
    }

    // Opens file to transmit
    if ((file_fd = open(app->path, O_RDONLY)) < 0){
        perror("Error opening file.");
        return -1;
    }

    // Sends START packet
    if (sendControlPacket(fd, START_PACKET, file_stat.st_size, app->path) < 0) {
        perror("Error sending START packet.");
        return -1;
    }

    // Sends DATA packets
    char buf[MAX_PACKET_SIZE], dataPacket[MAX_PACKET_SIZE];
    unsigned bytes_to_send;
    unsigned sequenceNumber = 0;
    unsigned progress = 0;

    while ((bytes_to_send = read(file_fd, buf, MAX_PACKET_SIZE - 4)) > 0) {
        dataPacket[0] = DATA_PACKET;
        dataPacket[1] = sequenceNumber % 255;
        dataPacket[2] = (bytes_to_send / 256);
        dataPacket[3] = (bytes_to_send % 256);
        memcpy(&dataPacket[4], buf, bytes_to_send);

        progress += bytes_to_send;
        printProgressBar(progress, file_stat.st_size);
        if (llwrite(fd, dataPacket, ((bytes_to_send + 4) < MAX_PACKET_SIZE)? (bytes_to_send + 4) : MAX_PACKET_SIZE) < 0) { // Only sends max packet if the last packet is of that size
            perror("llwrite failed");
            return -1;
        }

        // printf("Sent %d data bytes\n", bytes_to_send+4);

        memset(dataPacket, 0, MAX_PACKET_SIZE);
        sequenceNumber++;
        clearProgressBar();
    }

    printProgressBar(1, 1);
    printf("Data packets sent: %d\n",sequenceNumber);

    // Sends END packet
    if (sendControlPacket(fd, END_PACKET, file_stat.st_size, app->path) < 0) {
        perror("Error sending END packet.");
        return -1;
    }

    return 0;
}

int receiverApplication(int fd, applicationArgs * app) {
    char buf[MAX_PACKET_SIZE];
    int res;
    int nump = 0;

    while (1) {
        res = llread(fd, buf);
        nump++;

        // TODO add alarm for timeout

        if (parsePacket(buf, res, app) == END_PACKET) 
            break;

        memset(buf, 0, MAX_PACKET_SIZE); // Resets buffer
    }

    printf("Received %d packets\n", nump);
    return 0;
}

int parsePacket(char * buffer, int lenght, applicationArgs * app) {
    static int destinationFile;

    if (buffer[0] == START_PACKET) {
        parseControlPacket(buffer, lenght, app);
        destinationFile = open(app->path, O_RDWR | O_CREAT, 0777);
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

void parseControlPacket(char * buffer, int lenght, applicationArgs * app) {
    unsigned fileSize = 0;

    for (int i = 1; i < lenght; i++) {
        if (buffer[i] == FILE_SIZE) {
            i++; // i is now in the byte with information about the number of bytes
            for (int j = 0; j < buffer[i]; j++) {
                fileSize |= (buffer[i+j+1] << (8*j));
            }
            i += buffer[i];
            app->fileSize = fileSize;
        }

        if (buffer[i] == FILE_NAME) {
            i++; // i is now in the byte with information about the number of bytes
            strcat(app->path, &buffer[i+1]);
            i += buffer[i];
        }
    }
}

int sendControlPacket(int fd, char ctrl_field, unsigned file_size, char* file_name) {
    unsigned L1 = sizeof(file_size);
    unsigned L2 = strlen(file_name);
    unsigned packet_size = 5 + L1 + L2;

    char packet[packet_size];
    packet[0] = ctrl_field;
    packet[1] = FILE_SIZE;
    packet[2] = L1;
    memcpy(&packet[3], &file_size, L1);
    packet[3+L1] = FILE_NAME;
    packet[4+L1] = L2;
    memcpy(&packet[5+L1], file_name, L2);

    return llwrite(fd, packet, packet_size);
}

int main(int argc, char** argv)
{
    applicationArgs app;
    app.port = -1;
    app.role = -1;
    app.path[0] = '\0';

    if (checkArgs(argc, argv, &app) < 0) {
        perror("Usage: ./app -p <port> (-r | -t) <path>");
        return -1;
    }

    printf("App initialized!\nPort: %d\nRole: %d\nPath: %s\n", app.port, app.role, app.path);

    int fd;

    if ((fd = llopen(app.port, app.role)) < 0) {
        perror("llopen failed");
        return -1;
    }

    if (app.role == TRANSMITTER) {
        if (transmitterApplication(fd, &app) < 0) {
            perror("Transmitter Application failed");
            return -1;
        }
    }
    else {
        if (receiverApplication(fd, &app) < 0) {
            perror("Receiver Application failed");
            return -1;
        }
    }
    
    if (llclose(fd) < 0){
        perror("llclose failed");
        return -1;
    }

    return 0;
}