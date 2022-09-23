#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int write_helper(int fd, size_t message_size, size_t buffer_size){
    char* arr = (char*) malloc(message_size * sizeof(char));
    memset(arr, 100, message_size-1);
    arr[message_size-1] = '\0';

    size_t tmp, write_cnt = 0;
    while (message_size - write_cnt > buffer_size) {
        tmp = write(fd, arr+write_cnt, buffer_size);
        if (tmp > 0){
            write_cnt += tmp;
        }
    }
    tmp = write(fd, arr+write_cnt, message_size - write_cnt);
    return write_cnt+tmp;
}

int read_helper(int fd, size_t message_size, size_t buffer_size) {
    char* arr = (char*) malloc(message_size * sizeof(char));

    size_t tmp, read_cnt = 0;
    while (message_size - read_cnt > buffer_size){
        tmp = read(fd, arr+read_cnt, buffer_size);
        if (tmp > 0){
            read_cnt+=tmp;
        }
    }
    tmp = read(fd, arr+read_cnt, message_size - read_cnt);
    return read_cnt+tmp;
}