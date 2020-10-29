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

        perror("Hello");
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
    struct stat fileStat;

    if (stat(app->path, &fileStat)<0){
        perror("Error getting file information.");
        return -1;
    }

    if ((file_fd = open(app->path, O_RDONLY)) < 0){
        perror("Error opening file.");
        return -1;
    }
}

int receiverApplication(int fd, applicationArgs * app) {
    // RECEIVER APP
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
            perror("Transmitter Application failed");
            return -1;
        }
    }
    
    if (llclose(fd) < 0){
        perror("llclose failed");
        return -1;
    }

    return 0;
}