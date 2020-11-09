#include "application.h"

int main(int argc, char** argv)
{
    applicationArgs app;
    app.port = -1;
    app.role = -1;
    app.path[0] = '\0';

    if (checkArgs(argc, argv, &app) < 0) {
        fprintf(stderr, "Invalid args. Usage: ./app -p <port> (-r | -t) <path>");
        return -1;
    }

    printf("App initialized!\nPath: %s\n", app.path);

    int fd;

    if ((fd = llopen(app.port, app.role)) < 0) {
        fprintf(stderr, "llopen failed\n");
        return -1;
    }

    // Start Clock
    struct timeval beginTime, endTime;
    gettimeofday(&beginTime, NULL);

    if (app.role == TRANSMITTER) {
        if (transmitterApplication(fd, app.path) < 0) {
            fprintf(stderr, "Transmitter Application failed\n");
            return -1;
        }
    }
    else {
        if (receiverApplication(fd, app.path) < 0) {
            fprintf(stderr, "Receiver Application failed\n");
            return -1;
        }
    }

    // Get Elapsed Time
    gettimeofday(&endTime, NULL);
    
    double elapsed = (endTime.tv_sec - beginTime.tv_sec) * 1e6;
    elapsed = (elapsed + (endTime.tv_usec - beginTime.tv_usec)) * 1e-6;

    printf("Elapsed: %.5lf seconds\n", elapsed);
    
    if (llclose(fd) < 0){
        fprintf(stderr, "llclose failed\n");
        return -1;
    }

    return 0;
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
            if (atoi(argv[i+1]) >= 0){
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

    return 0;
}
