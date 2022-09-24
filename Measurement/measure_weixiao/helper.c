#include "helper.h"

uint64_t write_helper(int fd, size_t message_size, size_t buffer_size, uint64_t (*timer_func)()){
    char* arr = (char*) malloc(message_size * sizeof(char));
    memset(arr, 100, message_size-1);
    arr[message_size-1] = '\0';

    uint64_t start_time = 0;
    uint64_t end_time = 0;

    size_t tmp, write_cnt = 0;
    if (timer_func != NULL)
        start_time = (*timer_func)();
    while (message_size - write_cnt > buffer_size) {
        tmp = write(fd, arr+write_cnt, buffer_size);
        if (tmp > 0){
            write_cnt += tmp;
        }
    }
    tmp = write(fd, arr+write_cnt, message_size - write_cnt);
    if (timer_func != NULL)
        end_time = (*timer_func)();
    free(arr);
    return end_time - start_time;
}

uint64_t read_helper(int fd, size_t message_size, size_t buffer_size, uint64_t (*timer_func)()) {
    char* arr = (char*) malloc(message_size * sizeof(char));

    uint64_t start_time = 0;
    uint64_t end_time = 0;

    uint64_t tmp, read_cnt = 0;
    if (timer_func != NULL)
        start_time = (*timer_func)();
    while (message_size - read_cnt > buffer_size){
        tmp = read(fd, arr+read_cnt, buffer_size);
        if (tmp > 0){
            read_cnt+=tmp;
        }
    }
    tmp = read(fd, arr+read_cnt, message_size - read_cnt);
    if (timer_func != NULL)
        end_time = (*timer_func)();
    free(arr);
    return end_time - start_time;
}
