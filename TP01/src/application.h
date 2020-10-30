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

#define MAX_PACKET_SIZE (MAX_BUFFER_SIZE / 2)

#define DATA_PACKET 1
#define START_PACKET 2
#define END_PACKET 3

#define FILE_SIZE 0
#define FILE_NAME 1

#define PROGRESS_BAR_SIZE   30
#define SEPARATOR_CHAR      '='
#define NUM_BACKSPACES      PROGRESS_BAR_SIZE + 9

typedef struct {
    int port;
    int role;
    char path[256];
    unsigned fileSize;
} applicationArgs;

int checkArgs(int argc, char ** argv, applicationArgs * app);

int transmitterApplication(int fd, applicationArgs * app);

int receiverApplication(int fd, applicationArgs * app);

int parsePacket(char * buffer, int lenght, applicationArgs * app);

void parseControlPacket(char * buffer, int lenght, applicationArgs * app);

int sendControlPacket(int fd, char ctrl_field, unsigned file_size, char* file_name);