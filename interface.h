#include <stddef.h>

#ifndef INTERFACE_H
#define INTERFACE_H

#define MAX_BUFFER_CAP 1000000
#define TEST_BUFFER_SZ 100000

/* Headers for reporting rdtsc cycles
 * Usage - 
 * int tick_start = getticks();
 * int tick_end = getticks();
 * cycles taken: (tick_end - tick_start)/2.0 [accounting for RTT]
 * time taken ~ (tick_end - tick_start)/(2.0*cpu_freq_hz)
 */

typedef unsigned long long ticks;
// change this variable based on the evaluation system, cat /proc/cpuinfo | grep 'cpu MHz'
// put the value in Hz (grep value * 1000) 
int cpu_freq_hz = 1200887;

/* ticks code taken from https://www.unix.com/programming/81639-rdtsc-use-c.html
*/
static __inline__ ticks getticks(void)
{
    unsigned a, d;
    asm("cpuid");
    asm volatile("rdtsc" : "=a" (a), "=d" (d));

    return (((ticks)a) | (((ticks)d) << 32));
}

/**
 * @return 1 for success, 0 o/w
 * @param message_size in bytes
 * @param package_size in bytes
 */
int run_pipe(size_t message_size, size_t package_size);
int run_socket(size_t message_size, size_t package_size);
int run_shared_mem(size_t message_size, size_t package_size);
#endif