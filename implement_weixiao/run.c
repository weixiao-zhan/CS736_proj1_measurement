#include <stdio.h>
#include "../interface.h"


int run_pipe(size_t message_size, size_t package_size);
int run_socket(size_t message_size, size_t package_size);
int run_shared_mem(size_t message_size, size_t package_size);