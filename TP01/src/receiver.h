#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "ll.h"
#include "packet.h"

int receiverApplication(int fd, char* path);

int parsePacket(unsigned char * buffer, int lenght, char* path);

void parseControlPacket(unsigned char * buffer, int lenght, char* path, int* filesize);

int checkFileSize(char* path, int filesize);

int checkSequenceNumber(unsigned r_sn);