#include <stddef.h>

#ifndef INTERFACE_H
#define INTERFACE_H


#define S_4B    4
#define S_16B   16
#define S_64B   64
#define S_256B  256
#define S_1KB   1024
#define S_4KB   1024*4
#define S_16KB  1024*16
#define S_64KB  1024*64
#define S_256KB 1024*256
#define S_512KB 1024*512
#define S_1MB   1024*1024


//4, 16, 64, 256, 1K, 4K, 16K, 64K, 256K, and 512K bytes.


/**
 * @return 1 for success, 0 o/w
 * @param message_size in bytes
 * @param package_size in bytes
 */
int run_pipe(size_t message_size, size_t package_size);
int run_socket(size_t message_size, size_t package_size);
int run_shared_mem(size_t message_size, size_t package_size);
#endif