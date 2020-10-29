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

    if (stat(app->path, &file_stat)<0){
        perror("Error getting file information.");
        return -1;
    }

    if ((file_fd = open(app->path, O_RDONLY)) < 0){
        perror("Error opening file.");
        return -1;
    }

    if (sendControlPacket(fd, START_PACKET, file_stat.st_size, app->path) < 0) {
        perror("Error sending START packet.");
        return -1;
    }

    char buf[16];

    while (read(file_fd, buf, 16) > 0) {
        if (llwrite(fd, buf, 16) < 0) {
            perror("llwrite failed");
            return -1;
        }
    }

    if (sendControlPacket(fd, END_PACKET, file_stat.st_size, app->path) < 0) {
        perror("Error sending END packet.");
        return -1;
    }

    return 0;
}

int receiverApplication(int fd, applicationArgs * app) {
    // RECEIVER APP
    return 0;
}

int sendControlPacket(int fd, char ctrl_field, unsigned file_size, char* file_name) {
    unsigned L1 = sizeof(file_size);
    unsigned L2 = strlen(file_name);
    unsigned packet_size = 5 + L1 + L2;

    char packet[packet_size];
    packet[0] = ctrl_field;
    packet[1] = FILE_SIZE;
    printf("filesize: %d\n", file_size);
    packet[2] = L1;
    memcpy(&packet[3], &file_size, L1);
    packet[3+L1] = FILE_NAME;
    packet[4+L1] = L2;
    memcpy(&packet[5+L1], file_name, L2);

    return llwrite(fd, packet, packet_size);
    // for (int i = 0; i < packet_size; i++) {
    //     printf("pi:%#4.2x ", packet[i]);
    // }
    // printf("\n");
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