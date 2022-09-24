#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include "../interface.h"
#include "helper.h"

uint64_t run_socket(size_t message_size, size_t package_size, uint64_t (*timer_func)(), int mode){
    char *ip = "127.0.0.1";
    int port = 222228;
    uint64_t time_used = 0;
    size_t returning_message_size = message_size;
    if (mode != LATENCY)
        returning_message_size = 1;
    int fd = open("./sem_file", O_RDWR);
    sem_t* sem_server_ready = (sem_t*) mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, fd, 0);
    sem_init(sem_server_ready, 1, 0);

    pid_t childpid;
    if ((childpid = fork()) == -1) {
        perror("fork");
        return(-1);
    }
    

    if (childpid == 0) { /* Child process : server */

        int server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket < 0){
            perror("socket error");
            return -1;
        }

        int sock_closer = 1;
        setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &sock_closer, sizeof(int));
        setsockopt(server_socket, IPPROTO_TCP, TCP_NODELAY, &sock_closer, sizeof(int));
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = port;
        server_addr.sin_addr.s_addr = inet_addr(ip);
    
        if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
            perror("bind error");
            return -1;
        }
        if (listen(server_socket, 1) < 0) {
            printf("listen error\n");
            return -1;
        }
        sem_post(sem_server_ready);

        while(1){
            struct sockaddr_in client_addr;
            socklen_t addr_size = sizeof(client_addr);
            int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
            if (client_socket < 0) {
                continue;
            }

            read_helper(client_socket, message_size, package_size, NULL);
            write_helper(client_socket, returning_message_size, package_size, NULL);
            
            close(client_socket);
            break;
        }
        shutdown(server_socket, 2);
        //return 0;
        exit(0);
    } else { /* Parent process : client */
        sem_wait(sem_server_ready);
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0){
            perror("[-]Socket error");
            exit(1);
        }
        int sock_closer = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &sock_closer, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &sock_closer, sizeof(int));
        struct sockaddr_in addr; 
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = port;
        addr.sin_addr.s_addr = inet_addr(ip);
        
        if(connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0){
            perror("[-]connect error");
            exit(1);
        }
        uint64_t time_write = write_helper(sock, message_size, package_size, timer_func);
        uint64_t time_read = read_helper(sock, returning_message_size, package_size, timer_func);
        time_used = time_write + time_read;
        waitpid(childpid, NULL, 0);
        close(sock);
    }
    return time_used;
}