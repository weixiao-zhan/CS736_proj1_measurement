#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "../interface.h"
#include "helper.c"

int main(int argc, char** argv) {
    int message_size;
    int buffer_size = 64;
    if (argc >= 2){
        message_size = atoi(argv[1]);
    }
    if (argc >= 3){
        buffer_size = atoi(argv[2]);
    }
    run_pipe(message_size, buffer_size);
}

int pipe_setup(int* fd1, int* fd2){
    pipe(fd1);
    pipe(fd2);
    return 1;
}

int run_pipe(size_t message_size, size_t package_size){
    int fd1[2], fd2[2];
    pipe_setup(fd1, fd2);

    pid_t childpid;
    if ((childpid = fork()) == -1) {
        perror("fork");
        return(-1);
    }

    if (childpid == 0) { /* Child process */
        read_helper(fd1[0], message_size, package_size);
        write_helper(fd2[1], message_size, package_size);
        exit(0);
    } else { /* Parent process */
        write_helper(fd1[1], message_size, package_size);
        int n = read_helper(fd2[0], message_size, package_size);
        printf("Received: %d", n);
    }
    return(0);
}