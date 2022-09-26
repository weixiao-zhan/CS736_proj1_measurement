#include <stddef.h>
#include <stdint.h>

#ifndef INTERFACE_H
#define INTERFACE_H

//4, 16, 64, 256, 1K, 4K, 16K, 64K, 256K, and 512K bytes.
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
#define S_2MB   1024*2048

#define MAX_BUFFER_CAP 1000000
#define TEST_BUFFER_SZ 100000

#define THROUGHPUT 1
#define LATENCY 2 // so that mode value can be used for dividing by 2

/**
 * @return timer elapse value for success, 0 o/w
 * @param message_size in bytes
 * @param package_size in bytes
 */
uint64_t run_pipe(size_t message_size, size_t package_size, uint64_t (*timer_func)(), int mode);
uint64_t run_socket(size_t message_size, size_t package_size, uint64_t (*timer_func)(), int mode);
uint64_t run_shared_mem(size_t message_size, size_t package_size, uint64_t (*timer_func)(), int mode);


#endif
