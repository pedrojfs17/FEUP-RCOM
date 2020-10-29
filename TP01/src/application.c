#include "application.h"

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
    char buf[MAX_PACKET_SIZE];
    unsigned bytes_to_send = file_stat.st_size;
    while (read(file_fd, buf, MAX_PACKET_SIZE) > 0) {
        if (llwrite(fd, buf, (bytes_to_send < MAX_PACKET_SIZE)? bytes_to_send : MAX_PACKET_SIZE) < 0) { // Only sends max packet if the last packet is of that size
            perror("llwrite failed");
            return -1;
        }
        memset(buf, 0, MAX_PACKET_SIZE);
        bytes_to_send -= MAX_PACKET_SIZE;
    }

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

    // Read START packet
    if ((res = llread(fd, buf)) < 0) {
        perror("llread failed");
        return -1;
    }

    // Calculates number of packets to read
    unsigned num_bytes = 0;
    for (int i = 0; i < buf[2]; i++) {
        num_bytes |= (buf[3+i] << (i*8)); // Reads number from buffer
    }
    printf("Will read %d bytes\n", num_bytes);
    int num_packets = (num_bytes / MAX_PACKET_SIZE) + ((num_bytes % MAX_PACKET_SIZE)? 1 : 0);
    printf("Will read %d packets\n", num_packets);

    memset(buf, 0, MAX_PACKET_SIZE); // Resets buffer

    // Reads DATA packets
    for (int i = 0; i < num_packets; i++) {
        res = llread(fd, buf);

        // Prints read message
        printf("Read %d bytes. Message: '", res);
        for (int i = 0; i < res; i++) {
            printf("%c", buf[i]);
        }
        printf("'\n");

        memset(buf, 0, MAX_PACKET_SIZE); // Resets buffer
    }

    // Reads END packet
    if ((res = llread(fd, buf)) < 0) {
        perror("llread failed");
        return -1;
    }

    return 0;
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