#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <algorithm> 
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "../interface.h"

using namespace std;

#define SOCKET_NAME "/tmp/9Lq7BNBnBycd6nxy.socket"

struct pc_buffer {
     sem_t sem_prod;
     sem_t sem_cons;
     sem_t sem_switch;
     int prod_idx = 0;
     int cons_idx = 0;
     char buffer[BUFFER_SZ];
     int cap = BUFFER_SZ;
};

int run_pipe(size_t data_size, size_t buffer_sz /*unused*/) {
     printf("testing pipe communication with datasize=%lu\n", data_size);

     int fd1[2], fd2[2];
     if (pipe(fd1) == -1 || pipe(fd2) == -1) {
          printf("creation of pipe failed\n");

          // returning error
          return 0;
     }

     int child_id = fork();
     char* arr = (char*) malloc(data_size * sizeof(char));
     for (int i = 0 ; i + 1 < data_size ; i++) arr[i] = '0';
     arr[data_size - 1] = '\0';
     if (child_id != 0) {
          close(fd1[0]);
          close(fd2[1]);
          int bytes_written = 0, total_data_sent = 0;
          int bytes_read = 0, total_data_recv = 0;
          
          int tick_start = getticks();
          while (total_data_sent < data_size) {
               bytes_written = write(fd1[1], arr, data_size);
               if (bytes_written >= 0) total_data_sent += bytes_written;
          }
          close(fd1[1]);
          
          while(bytes_read = read(fd2[0], arr + bytes_read, data_size)) {
               if (bytes_read >= 0) total_data_recv += bytes_read;
          }
          int tick_end = getticks();

          printf("ticks taken for pipe communication of %lu bytes=%f, time in ms=%f\n", 
                    data_size, 
                    (tick_end - tick_start)/2.0,
                    (tick_end - tick_start)/(2.0*cpu_freq_hz));
     } else {
          close(fd1[1]);
          close(fd2[0]);

          int bytes_written = 0, total_data_sent = 0;
          int bytes_read = 0, total_data_recv = 0;
          
          while(bytes_read = read(fd1[0], arr + bytes_read, data_size)) {
               if (bytes_read >= 0) total_data_recv += bytes_read;
          }
          
          while (total_data_sent < data_size) {
               bytes_written = write(fd2[1], arr, data_size);
               if (bytes_written >= 0) total_data_sent += bytes_written;
          }
          close(fd2[1]);
     }

     return 1;
}

int socket_server(int data_size) {
     unlink(SOCKET_NAME);

     int fd = socket(AF_UNIX, SOCK_STREAM, 0);
     if (fd < 0) {
          printf("socket creation error\n");
          // returning error
          return 0;
     }

     struct sockaddr_un saddr;
     memset(&saddr, 0, sizeof(struct sockaddr_un));
     saddr.sun_family = AF_UNIX; 
     strncpy(saddr.sun_path, SOCKET_NAME, sizeof(saddr.sun_path) - 1);

     int sock_closer = 1;
     setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &sock_closer, sizeof(int));

     if (bind(fd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
          printf("socket bind error\n");
          // returning error
          return 0;
     }

     if (listen(fd, 1) < 0) {
          printf("socket listen error\n");
          // returning error
          return 0;
     }

     char *arr = (char*) malloc((data_size + 1)*sizeof(char));
     while(1) {
          struct sockaddr_in caddr; 
          int len = sizeof(caddr);
          int client_fd = accept(fd, (struct sockaddr*) &caddr, (socklen_t*) &len); 
          if (client_fd < 0) continue;

          int tick_start = getticks();
          int bytes_read = 0, bytes_written = 0, total_data_recv = 0, total_data_sent = 0;

          while(total_data_recv < data_size) {
               bytes_read = read(client_fd, arr + total_data_recv, data_size - total_data_recv);
               total_data_recv += bytes_read;
          }

          while (total_data_sent < total_data_recv) {
               bytes_written = write(client_fd, arr + total_data_sent, total_data_recv - total_data_sent);
               total_data_sent += bytes_written;
          }
          close(client_fd);
          int tick_end = getticks();
          
          printf("ticks taken for socket communication of %d bytes=%f, time in ms=%f\n", 
                    data_size, 
                    (tick_end - tick_start)/2.0,
                    (tick_end - tick_start)/(2.0*cpu_freq_hz));

          break;
     }
     close(fd);
     unlink(SOCKET_NAME);

     return 1;
}

void socket_client(int data_size) {
     int fd = socket(AF_UNIX, SOCK_STREAM, 0);
     if (fd < 0) {
          printf("socket creation error\n");
          exit(1);
     }

     int sock_closer = 1;
     setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &sock_closer, sizeof(int));

     struct sockaddr_un saddr;
     memset(&saddr, 0, sizeof(struct sockaddr_un));
     saddr.sun_family = AF_UNIX;
     strncpy(saddr.sun_path, SOCKET_NAME, sizeof(saddr.sun_path) - 1);

     if (connect(fd, (struct sockaddr*) &saddr, sizeof(saddr)) < 0) {
          printf("socket connect failed\n");
          exit(1);
     }

     char *arr = (char*) malloc((data_size + 1) * sizeof(char));
     for (int i = 0 ; i + 1 < data_size ; i++) arr[i] = '0';
     arr[data_size - 1] = '\0';

     int bytes_read = 0, bytes_written = 0, total_data_recv = 0, total_data_sent = 0;
     while (total_data_sent < data_size) {
          bytes_written = write(fd, arr + total_data_sent, data_size - total_data_sent);
          total_data_sent += bytes_written;
     }

     while(total_data_recv < total_data_sent) {
          bytes_read = read(fd, arr + total_data_recv, total_data_sent - total_data_recv);
          total_data_recv += bytes_read;
     }
     close(fd);
}

int run_socket(size_t data_size, size_t buffer_sz /*unused*/) {
     printf("testing socket communication with datasize=%lu\n", data_size);
     int ret = 1;
     int child_id = fork();
     if (child_id != 0) ret = socket_server(data_size);
     else {
          socket_client(data_size);
          exit(0);
     }

     waitpid(child_id, NULL, 0);

     // returning 1 for success, as per header file.
     return ret;
}

int run_shared_mem(size_t data_size, size_t buffer_sz /*unused*/) {
     printf("testing shared memory communication with datasize=%lu\n", data_size);

     int fd = open("/dev/zero", O_RDWR);
     pc_buffer *buffer;
     buffer = (pc_buffer*) mmap(NULL, sizeof(pc_buffer), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
     buffer->cap = BUFFER_SZ;
     sem_init(&buffer->sem_prod, 1, buffer->cap);
     sem_init(&buffer->sem_cons, 1, 0);
     sem_init(&buffer->sem_switch, 1, 0);
     buffer->cons_idx = 0;
     buffer->prod_idx = 0;
     

     char *arr = (char*) malloc(data_size*sizeof(char));
     for (int i = 0 ; i + 1 < data_size ; i++) arr[i] = '0';
     arr[data_size - 1] = '\0';
     int child_id = fork();
     if (child_id != 0) {
          int tick_start = getticks();

          int bytes_written = 0, bytes_read = 0, buffer_idx = 0;
          while(bytes_written < data_size) {
               sem_wait(&buffer->sem_prod);
               strncpy(buffer->buffer + buffer_idx, arr + bytes_written, min(buffer->cap, (int) data_size - bytes_written));
               bytes_written += min(buffer->cap, (int) data_size - bytes_written);
               buffer_idx = bytes_written % buffer->cap;
               sem_post(&buffer->sem_prod);
          }
          sem_wait(&buffer->sem_switch);
          buffer_idx = 0;
          while(bytes_read < data_size) {
               sem_wait(&buffer->sem_prod);
               strncpy(arr + bytes_read, buffer->buffer + buffer_idx, min(buffer->cap, (int) data_size - bytes_read));
               bytes_read += min(buffer->cap, (int) data_size - bytes_read);
               buffer_idx = bytes_read % buffer->cap;
               sem_post(&buffer->sem_prod);
          }

          int tick_end = getticks();
          printf("ticks taken for sm communication of %lu bytes=%f, time in ms=%f\n", 
                    data_size, 
                    (tick_end - tick_start)/2.0,
                    (tick_end - tick_start)/(2.0*cpu_freq_hz));
          
          munmap(buffer, sizeof(pc_buffer));

     } else {
          int bytes_written = 0, bytes_read = 0, buffer_idx = 0;
          while(bytes_read < data_size) {
               sem_wait(&buffer->sem_prod);
               strncpy(arr + bytes_read, buffer->buffer + buffer_idx, std::min(buffer->cap, (int) data_size - bytes_read));
               bytes_read += std::min(buffer->cap, (int) data_size - bytes_read);
               buffer_idx = bytes_read % buffer->cap;
               sem_post(&buffer->sem_prod);
          }
          sem_post(&buffer->sem_switch);
          buffer_idx = 0;
          while(bytes_written < data_size) {
               sem_wait(&buffer->sem_prod);
               strncpy(buffer->buffer + buffer_idx, arr + bytes_written, std::min(buffer->cap, (int) data_size - bytes_written));
               bytes_written += std::min(buffer->cap, (int) data_size - bytes_written);
               buffer_idx = bytes_written % buffer->cap;
               sem_post(&buffer->sem_prod);
          }
          exit(0);
     }

     waitpid(child_id, NULL, 0);

     // returning 1 for success, as per header file.
     return 1;
}

int main(int argc, char* argv[]) {
     if (argc != 3) {
          printf("Usage ./a.out <mode of communication> <data size>\n");
          exit(0);
     }

     int mode = 0;
     if (!strcmp(argv[1], "pipe")) {
          mode = 1;
     } else if (!strcmp(argv[1], "socket")) {
          mode = 2;
     } else if (!strcmp(argv[1], "sm")) {
          mode = 3;
     } else {
          printf("Supported modes - pipe, socket, and sm\n");
          exit(0);
     }

     switch(mode) {
          case 1: {
               run_pipe(atoi(argv[2]), BUFFER_SZ);
               break;
          }
          case 2: {
               run_socket(atoi(argv[2]), BUFFER_SZ);
               break;
          }
          case 3: {
               run_shared_mem(atoi(argv[2]), BUFFER_SZ);
          }
     }

     return 0;
}