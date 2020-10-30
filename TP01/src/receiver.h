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

int parsePacket(char * buffer, int lenght, char* path);

void parseControlPacket(char * buffer, int lenght, char* path);