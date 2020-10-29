#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include "ll.h"

#define DATA_PACKET 1
#define START_PACKET 2
#define END_PACKET 3

#define FILE_SIZE 0
#define FILE_NAME 1

typedef struct {
    int port;
    int role;
    char path[256];
} applicationArgs;

int checkArgs(int argc, char ** argv, applicationArgs * app);

int transmitterApplication(int fd, applicationArgs * app);

int receiverApplication(int fd, applicationArgs * app);

int sendControlPacket(int fd, char ctrl_field, unsigned file_size, char* file_name);