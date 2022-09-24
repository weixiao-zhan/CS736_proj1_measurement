#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#ifndef HELPER_H
#define HELPER_H

uint64_t write_helper(int fd, size_t message_size, size_t buffer_size, uint64_t (*timer_func)());

uint64_t read_helper(int fd, size_t message_size, size_t buffer_size, uint64_t (*timer_func)());


#endif