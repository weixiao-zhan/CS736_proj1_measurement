#include <stddef.h>

#ifndef INTERFACE_H
#define INTERFACE_H
/**
 * @return 1 for success, 0 o/w
 * @param message_size in bytes
 * @param package_size in bytes
 */
int run_pipe(size_t message_size, size_t package_size);
int run_socket(size_t message_size, size_t package_size);
int run_shared_mem(size_t message_size, size_t package_size);
#endif