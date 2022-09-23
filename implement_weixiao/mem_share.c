#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <string.h>
#include "../interface.h"
#include "helper.c"

#define MIN(a,b) (((a)<(b))?(a):(b))

typedef struct {
    sem_t sem_writing_lock;
    sem_t sem_buffer_ready;
    sem_t sem_switch;
    int prod_idx;
    int cons_idx;
    char buffer[MAX_BUFFER_CAP];
} shared_buffer;

int main(int argc, char** argv) {
    int message_size;
    int buffer_size = 64;
    if (argc >= 2){
        message_size = atoi(argv[1]);
    }
    if (argc >= 3){
        buffer_size = atoi(argv[2]);
    }
    run_shared_mem(message_size, buffer_size);
}


int run_shared_mem(size_t data_size, size_t buffer_size) {
    int fd = open("./sm_file", O_RDWR);
    shared_buffer* buffer = (shared_buffer*) mmap(NULL, sizeof(shared_buffer), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, fd, 0);
    if ((void*)buffer == MAP_FAILED) { perror("mmap");  exit(-1); }
    sem_init(&buffer->sem_writing_lock, 1, 1);
    sem_init(&buffer->sem_switch, 1, 0);
    buffer->cons_idx = 0;
    buffer->prod_idx = 0;
    
    int child_id = fork();
    if (child_id == 0) {
        char *arr = (char*) malloc(data_size*sizeof(char));

        int write_cnt = 0, read_cnt = 0, buffer_idx = 0;
        while(read_cnt < data_size) {
            sem_wait(&buffer->sem_writing_lock);
            int n = MIN((int) buffer_size-buffer_idx, (int) data_size - read_cnt);
            strncpy(arr + read_cnt, buffer->buffer + buffer_idx, n);
            read_cnt += n;
            buffer_idx = (buffer_idx+n) % buffer_size;
            sem_post(&buffer->sem_writing_lock);
        }
        sem_post(&buffer->sem_switch);
        buffer_idx = 0;
        while(write_cnt < data_size) {
            sem_wait(&buffer->sem_writing_lock);
            int n = MIN((int) buffer_size-buffer_idx, (int) data_size - write_cnt);
            strncpy(buffer->buffer + buffer_idx, arr + write_cnt, n);
            write_cnt += n;
            buffer_idx = (buffer_idx+n) % buffer_size;
            sem_post(&buffer->sem_writing_lock);
            //printf("c: %d\n", write_cnt);
        }
        return 1;
    } else {
        char *arr = (char*) malloc(data_size*sizeof(char));
        memset(arr, '1', data_size-1);
        arr[data_size - 1] = '\0';
        int write_cnt = 0, read_cnt = 0, buffer_idx = 0;
        while(write_cnt < data_size) {
            sem_wait(&buffer->sem_writing_lock);
            int n = MIN((int) buffer_size-buffer_idx, (int) data_size - write_cnt);
            strncpy(buffer->buffer + buffer_idx, arr + write_cnt, n);
            write_cnt += n;
            buffer_idx = (buffer_idx+n) % buffer_size;
            sem_post(&buffer->sem_writing_lock);
        }
        sem_wait(&buffer->sem_switch);
        buffer_idx = 0;
        while(read_cnt < data_size) {
            sem_wait(&buffer->sem_writing_lock);
            int n = MIN((int) buffer_size-buffer_idx, (int) data_size - read_cnt);
            strncpy(arr + read_cnt, buffer->buffer + buffer_idx, n);
            read_cnt += n;
            buffer_idx = (buffer_idx+n) % buffer_size;
            sem_post(&buffer->sem_writing_lock);
        }
        printf("Recieved: %s\n(%d)", arr, read_cnt);
        munmap(buffer, sizeof(shared_buffer));
        waitpid(child_id, NULL, 0);
    }
    // returning 1 for success, as per header file.
    return 1;
}