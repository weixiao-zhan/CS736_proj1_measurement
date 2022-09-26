#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "../interface.h"
#include "helper.h"

int pipe_setup(int* fd1, int* fd2){
    pipe(fd1);
    pipe(fd2);
    return 1;
}

uint64_t run_pipe(size_t message_size, size_t package_size, uint64_t (*timer_func)(), int mode){
    int fd1[2], fd2[2];
    uint64_t time_used = 0;
    size_t returning_message_size = message_size;
    if (mode != LATENCY)
        returning_message_size = 1;
    pipe_setup(fd1, fd2);
    
    pid_t childpid;
    if ((childpid = fork()) == -1) {
        perror("fork");
        return(-1);
    }

    if (childpid == 0) { /* Child process */
        read_helper(fd1[0], message_size, package_size, NULL);
        write_helper(fd2[1], returning_message_size, package_size, NULL);
        exit(0);
    } else { /* Parent process */
        uint64_t time_write = write_helper(fd1[1], message_size, package_size, timer_func);
        uint64_t time_read = read_helper(fd2[0], returning_message_size, package_size, timer_func);
        time_used = time_write + time_read;
    }
    close(fd1[0]);
    close(fd1[1]);
    close(fd2[0]);
    close(fd2[0]);
    return(time_used);
}