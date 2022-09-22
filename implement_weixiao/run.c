#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "../interface.h"

int main(int argc, char** argv) {
    int message_size=1024*8; // this implementation is limited to 8kB
    int buffer_size = 64;
    if (argc >= 2){
        message_size = atoi(argv[1]);
    }
    if (argc >= 3){
        buffer_size = atoi(argv[2]);
    }
    run_pipe(message_size, buffer_size);
}

int write_helper(int fd, size_t message_size, size_t buffer_size){
    char buf[buffer_size];
    memset(buf, 100, buffer_size);
    size_t writen = 0;
    while(writen < message_size){
        writen += write(fd, buf, buffer_size);
    }
    return writen;
}

int read_helper(int fd, size_t message_size, size_t buffer_size) {
    char buf[buffer_size];
    size_t readed = 0;
    while (readed < message_size){
        readed += read(fd, buf, buffer_size);
    }
    return readed;
}

int run_pipe(size_t message_size, size_t package_size){
    int fd[2];
    pipe(fd);

    pid_t childpid;
    if ((childpid = fork()) == -1) {
        perror("fork");
        return(-1);
    }

    if (childpid == 0) { /* Child process */
        read_helper(fd[0], message_size, package_size);
        write_helper(fd[1], message_size, package_size);
        exit(0);
    } else { /* Parent process */
        write_helper(fd[1], message_size, package_size);
        int n = read_helper(fd[0], message_size, package_size);
        printf("Received: %d", n);
    }
    return(0);
}

int run_socket(size_t message_size, size_t package_size);
int run_shared_mem(size_t message_size, size_t package_size);