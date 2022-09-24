#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include "../interface.h"
#include "helper.c"

#define MIN(a,b) (((a)<(b))?(a):(b))

typedef struct {
    sem_t sem_buffer_lock;
    uint using_size;
    sem_t sem_switch;
    char buffer[MAX_BUFFER_CAP];
} shared_buffer;

int main(int argc, char** argv) {
    int data_size;
    int buffer_size = 64;
    if (argc >= 2){
        data_size = atoi(argv[1]);
    }
    if (argc >= 3){
        buffer_size = atoi(argv[2]);
    }
    run_shared_mem(data_size, buffer_size);
}

int run_shared_mem(size_t data_size, size_t buffer_size){
    char data[data_size];
    for(int i =0; i < data_size-1; i++){
        data[i] = 'a'+(i%26);
    }
    data[data_size-1] = '\0';
    return _run_shared_mem(data, data_size, buffer_size);
}

int _run_shared_mem(char* data, size_t data_size, size_t buffer_size) {
    int fd = open("./sm_file", O_RDWR);
    shared_buffer* buffer = (shared_buffer*) mmap(NULL, sizeof(shared_buffer), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, fd, 0);
    if ((void*)buffer == MAP_FAILED) { perror("mmap");  exit(-1); }
    sem_init(&buffer->sem_buffer_lock, 1, 1);
    buffer->using_size = 0;
    sem_init(&buffer->sem_switch, 1, 0);
    
    int child_id = fork();
    if (child_id == 0) { // child process
        char* data_received  = (char*) malloc(data_size*sizeof(char));
        int write_cnt = 0, read_cnt = 0, buffer_idx = 0;

        while(read_cnt < data_size) {
            sem_wait(&buffer->sem_buffer_lock);
            int n = MIN(MIN((int) buffer_size-buffer_idx, (int) data_size - read_cnt), buffer->using_size);
            if (n > 0){
                memcpy(data_received + read_cnt, buffer->buffer + buffer_idx, n);
                read_cnt += n;
                buffer_idx = (buffer_idx+n) % buffer_size;
                buffer->using_size -= n;
            }
            sem_post(&buffer->sem_buffer_lock);
        }
        
        printf("C: Recieved: %s\n(%d)\n", data_received, read_cnt);
        // read complete
        buffer->using_size = 0; // reset buffer
        sem_post(&buffer->sem_switch);

        //start to write
        buffer_idx = 0;
        while(write_cnt < data_size) {
            sem_wait(&buffer->sem_buffer_lock);
            int n = MIN(MIN((int) buffer_size-buffer_idx, (int) data_size - write_cnt), buffer_size - buffer->using_size);
            if(n > 0){
                memcpy(buffer->buffer + buffer_idx, data_received + write_cnt, n);
                write_cnt += n;
                buffer_idx = (buffer_idx+n) % buffer_size;
                buffer->using_size += n;
            }
            sem_post(&buffer->sem_buffer_lock);
        }

        return 1;
    } else {
        char* data_received  = (char*) malloc(data_size*sizeof(char));
        int write_cnt = 0, read_cnt = 0, buffer_idx = 0;

        // @HERE: start timer

        while(write_cnt < data_size) {
            sem_wait(&buffer->sem_buffer_lock);
            int n = MIN(MIN((int) buffer_size-buffer_idx, (int) data_size - write_cnt), buffer_size - buffer->using_size);
            if(n > 0){
                memcpy(buffer->buffer + buffer_idx, data + write_cnt, n);
                write_cnt += n;
                buffer_idx = (buffer_idx+n) % buffer_size;
                buffer->using_size += n;
            }
            sem_post(&buffer->sem_buffer_lock);
        }
        sem_wait(&buffer->sem_switch);
        //write complete

        //start to read
        buffer_idx = 0;
        while(read_cnt < data_size) {
            sem_wait(&buffer->sem_buffer_lock);
            int n = MIN(MIN((int) buffer_size-buffer_idx, (int) data_size - read_cnt), buffer->using_size);
            if (n > 0){
                memcpy(data_received + read_cnt, buffer->buffer + buffer_idx, n);
                read_cnt += n;
                buffer_idx = (buffer_idx+n) % buffer_size;
                buffer->using_size -= n;
            }
            sem_post(&buffer->sem_buffer_lock);
        }

        // @HERE: stop timer

        printf("P: Recieved: %s\n(%d)\n", data_received, read_cnt);
        munmap(buffer, sizeof(shared_buffer));
        waitpid(child_id, NULL, 0);
    }
    // returning 1 for success, as per header file.
    return 1;
}