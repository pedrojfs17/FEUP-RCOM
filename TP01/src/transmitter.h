#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "ll.h"
#include "packet.h"
#include "utils.h"

int transmitterApplication(int fd, char* path);

int sendControlPacket(int fd, char ctrl_field, unsigned file_size, char* file_name);