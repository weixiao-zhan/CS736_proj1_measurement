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

#include <errno.h>

using namespace std;

#define SOCKET_NAME "/tmp/9Lq7BNBnBycd6nxy.socket"

struct pc_buffer {
     sem_t sem_prod;
     sem_t sem_cons;
     sem_t sem_switch;
     char buffer[MAX_BUFFER_CAP];
     int cap = MAX_BUFFER_CAP;
};

uint64_t run_pipe(size_t data_size, size_t buffer_sz, uint64_t (*timer_func)(), int mode) {
     int fd1[2], fd2[2];
     if (pipe(fd1) == -1 || pipe(fd2) == -1) {
          printf("creation of pipe failed %d\n", errno);

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
          
          uint64_t tick_start = (*timer_func)();
          while (total_data_sent < data_size) {
               bytes_written = write(fd1[1], arr + total_data_sent, min((int) buffer_sz, (int) data_size - total_data_sent));
               if (bytes_written >= 0) total_data_sent += bytes_written;
          }
          close(fd1[1]);
          
          if (mode == LATENCY){
               while(bytes_read = read(fd2[0], arr + total_data_recv, min((int) buffer_sz, (int) data_size - total_data_recv))) {
                    if (bytes_read >= 0) total_data_recv += bytes_read;
               }
          } else {
                while(bytes_read = read(fd2[0], arr, 1)) {
                    if (bytes_read >= 0) total_data_recv += bytes_read;
               }
          }
          uint64_t tick_end = (*timer_func)();
          free(arr);
          return tick_end - tick_start;
     } else {
          close(fd1[1]);
          close(fd2[0]);

          int bytes_written = 0, total_data_sent = 0;
          int bytes_read = 0, total_data_recv = 0;
          
          while(bytes_read = read(fd1[0], arr + total_data_recv, min((int) buffer_sz, (int) data_size - total_data_recv))) {
               if (bytes_read >= 0) total_data_recv += bytes_read;
          }
          
          if (mode == LATENCY){
               while (total_data_sent < data_size) {
                    bytes_written = write(fd2[1], arr + total_data_sent,  min((int) buffer_sz, (int) data_size - total_data_sent));
                    if (bytes_written >= 0) total_data_sent += bytes_written;
               }
          } else {
               write(fd2[1], arr, 1);
          }
          close(fd2[1]);
          exit(0);
     }
}

uint64_t socket_server(int data_size, size_t buffer_sz, uint64_t (*timer_func)(), int mode) {
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
     uint64_t ticks_counted = 0;
     while(1) {
          struct sockaddr_in caddr; 
          int len = sizeof(caddr);
          int client_fd = accept(fd, (struct sockaddr*) &caddr, (socklen_t*) &len); 
          if (client_fd < 0) continue;

          int bytes_read = 0, bytes_written = 0, total_data_recv = 0, total_data_sent = 0;
          uint64_t tick_start = (*timer_func)();
          while(total_data_recv < data_size) {
               bytes_read = read(client_fd, arr + total_data_recv, min((int) buffer_sz, (int) data_size - total_data_recv));
               total_data_recv += bytes_read;
          }

          if (mode == LATENCY){
               while (total_data_sent < total_data_recv) {
                    bytes_written = write(client_fd, arr + total_data_sent, min((int) buffer_sz, total_data_recv - total_data_sent));
                    total_data_sent += bytes_written;
               }
          } else {
               bytes_written = write(client_fd, arr, 1);
          }
          uint64_t tick_end = (*timer_func)();
          ticks_counted = tick_end - tick_start;
          close(client_fd);

          break;
     }
     close(fd);
     unlink(SOCKET_NAME);

     free(arr);
     return ticks_counted;
}

void socket_client(int data_size, size_t buffer_sz, int mode) {
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
          printf("socket connect failed %d\n", errno);
          exit(1);
     }

     char *arr = (char*) malloc((data_size + 1) * sizeof(char));
     for (int i = 0 ; i + 1 < data_size ; i++) arr[i] = '0';
     arr[data_size - 1] = '\0';

     int bytes_read = 0, bytes_written = 0, total_data_recv = 0, total_data_sent = 0;
     while (total_data_sent < data_size) {
          bytes_written = write(fd, arr + total_data_sent, min((int) buffer_sz, (int) data_size - total_data_sent));
          total_data_sent += bytes_written;
     }

     if (mode  == LATENCY){
          while(total_data_recv < total_data_sent) {
               bytes_read = read(fd, arr + total_data_recv, min((int) buffer_sz, total_data_sent - total_data_recv));
               total_data_recv += bytes_read;
          }
     } else {
          while(total_data_recv < 1) {
               bytes_read = read(fd, arr, 1);
               total_data_recv += bytes_read;
          }
     }
     close(fd);
     free(arr);
}

uint64_t run_socket(size_t data_size, size_t buffer_sz, uint64_t (*timer_func)(), int mode) {
     uint64_t ret = 1;
     int child_id = fork();
     if (child_id != 0) ret = socket_server(data_size, buffer_sz, timer_func, mode);
     else {
          socket_client(data_size, buffer_sz, mode);
          exit(0);
     }

     waitpid(child_id, NULL, 0);

     // returning 1 for success, as per header file.
     return ret;
}

uint64_t run_shared_mem(size_t data_size, size_t buffer_sz, uint64_t (*timer_func)(), int mode) {

     uint64_t ticks_counted = 0;
     int fd = open("/dev/zero", O_RDWR);
     pc_buffer *buffer;
     buffer = (pc_buffer*) mmap(NULL, sizeof(pc_buffer), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
     buffer->cap = MAX_BUFFER_CAP;
     sem_init(&buffer->sem_prod, 1, buffer->cap/buffer_sz);
     sem_init(&buffer->sem_cons, 1, 0);
     sem_init(&buffer->sem_switch, 1, 0);

     char *arr = (char*) malloc(data_size*sizeof(char));
     for (int i = 0 ; i + 1 < data_size ; i++) arr[i] = '0';
     arr[data_size - 1] = '\0';
     int child_id = fork();
     if (child_id != 0) {
          uint64_t tick_start = (*timer_func)();

          int bytes_written = 0, bytes_read = 0, buffer_idx = 0, bytes_to_write, bytes_to_read;
          while(bytes_written < data_size) {
               sem_wait(&buffer->sem_prod);
               bytes_to_write = min({(int) buffer_sz, (int) data_size - bytes_written, (int) buffer->cap - buffer_idx});
               strncpy(buffer->buffer + buffer_idx, arr + bytes_written, bytes_to_write);
               bytes_written += bytes_to_write;
               buffer_idx = bytes_written % buffer->cap;
               sem_post(&buffer->sem_cons);
          }
          sem_wait(&buffer->sem_switch);
          buffer_idx = 0;
          if (mode == LATENCY){
               while(bytes_read < data_size) {
                    sem_wait(&buffer->sem_cons);
                    bytes_to_read = min({(int) buffer_sz, (int) data_size - bytes_read, (int) buffer->cap - buffer_idx});
                    strncpy(arr + bytes_read, buffer->buffer + buffer_idx, bytes_to_read);
                    bytes_read += bytes_to_read;
                    buffer_idx = bytes_read % buffer->cap;
                    sem_post(&buffer->sem_prod);
               }
          } else {
               while(bytes_read < 1) {
                    sem_wait(&buffer->sem_cons);
                    bytes_to_read = 1;
                    strncpy(arr + bytes_read, buffer->buffer + buffer_idx, bytes_to_read);
                    bytes_read += bytes_to_read;
                    buffer_idx = bytes_read % buffer->cap;
                    sem_post(&buffer->sem_prod);
               }
          }

          uint64_t tick_end = (*timer_func)();
          ticks_counted = tick_end -tick_start;
          munmap(buffer, sizeof(pc_buffer));

     } else {
          int bytes_written = 0, bytes_read = 0, buffer_idx = 0, bytes_to_write, bytes_to_read;
          while(bytes_read < data_size) {
               sem_wait(&buffer->sem_cons);
               bytes_to_read = std::min({(int) buffer_sz, (int) data_size - bytes_read, (int) buffer->cap - buffer_idx});
               strncpy(arr + bytes_read, buffer->buffer + buffer_idx, bytes_to_read);
               bytes_read += bytes_to_read;
               buffer_idx = bytes_read % buffer->cap;
               sem_post(&buffer->sem_prod);
          }
          sem_post(&buffer->sem_switch);
          buffer_idx = 0;
          if (mode == LATENCY){
               while(bytes_written < data_size) {
                    sem_wait(&buffer->sem_prod);
                    bytes_to_write = std::min({(int) buffer_sz, (int) data_size - bytes_written, (int) buffer->cap - buffer_idx});
                    strncpy(buffer->buffer + buffer_idx, arr + bytes_written, bytes_to_write);
                    bytes_written += bytes_to_write;
                    buffer_idx = bytes_written % buffer->cap;
                    sem_post(&buffer->sem_cons);
               }
          } else {
               while(bytes_written < 1) {
                    sem_wait(&buffer->sem_prod);
                    bytes_to_write = 1;
                    strncpy(buffer->buffer + buffer_idx, arr + bytes_written, bytes_to_write);
                    bytes_written += bytes_to_write;
                    buffer_idx = bytes_written % buffer->cap;
                    sem_post(&buffer->sem_cons);
               }
          }
          exit(0);
     }

     waitpid(child_id, NULL, 0);
     free(arr);
     // returning 1 for success, as per header file.
     return ticks_counted;
}
