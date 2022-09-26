// TODO: Add error handling if needed
#include <time.h>
#include <sys/time.h>

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <inttypes.h>

#include "interface.h"

#define FREQ 2700000000
#define MAX_REP 10

uint64_t simple_operation(size_t a, size_t b, uint64_t (*timer_func)(), int mode);

//run_pipe, run_socket, run_shared_mem
uint64_t (*ipc_func[])(size_t, size_t, uint64_t (*timer_func)(), int) = {run_pipe, run_socket, run_shared_mem};
const char* ipc_func_name[] = { "pipe", "socket", "shared mem"};
int ipc_func_count = 3;


size_t buffer_size_arr[] = {S_4B, S_16B, S_64B, S_256B, S_1KB, S_4KB, S_16KB, S_64KB};

size_t datasize_arr[] = {S_4B, S_16B, S_64B, S_256B, S_1KB, S_4KB, S_16KB, S_64KB, S_256KB, S_512KB, S_1MB};

int datasize_arr_count = 11;
int buffer_size_arr_count = 8;


struct timespec global_timespec;
struct timeval global_timeval;
uint64_t first32 = 0;
uint64_t last32 = 0;

/*
* Operation to capture timer accuracy?
*/
uint64_t simple_operation(size_t a, size_t b, uint64_t (*timer_func)(), int mode){
    uint64_t before = (*timer_func)();
    size_t volatile res = a;
    for (int i = 0; i < b; i++)
        res = res*i % 123;
    uint64_t after = (*timer_func)();
    return after-before;
}

/*
* Return current CPU cycles count as timestamp
* Code modeled after https://www.unix.com/programming/81639-rdtsc-use-c.html
*/
uint64_t timestamp_rdtsc(){
    first32 = 0;
    last32 = 0;
    asm("cpuid");
    asm volatile("rdtsc \n\t" : "=a"(last32), "=d"(first32));
    return (first32<<32) | last32;
}

/*
* Return the nanoseconds count from CLOCK_MONOTONIC as timestamp
*/
uint64_t timestamp_clock_mono(){
    clock_gettime(CLOCK_MONOTONIC, &global_timespec);
    uint64_t time_nano= 1000000000 * global_timespec.tv_sec + global_timespec.tv_nsec;
    return time_nano;
}

/*
* Return the microseconds from gettimeofday 
*/
uint64_t timestamp_timeofday(){
    gettimeofday(&global_timeval, NULL);
    uint64_t time_micro= 1000000 * global_timeval.tv_sec + global_timeval.tv_usec;
    return time_micro;
}

uint64_t (*available_timer_funcs[])() = {timestamp_rdtsc, timestamp_clock_mono, timestamp_timeofday};

void perform_measurement(int ipc_func_index, int mode, uint64_t (*timer_func)()){
    // TODO: arrays: funcs, functs name, data_size_arr, packsize_arr_count
    uint64_t counter_transfer = 0;  // counter for only transfer part
    uint64_t counter_all = 0; // ccounter for the entire ipc function call
    uint64_t reportable_avg_transfer = 0;  // reportable average for only transfer part
    uint64_t reportable_avg_all = 0; // reportable average for the entire ipc function call
    printf("mechanism, data size, buffer/package size, how many reps for avg, Transfer time, Overall time\n");
    fflush(stdout);
    for (int d = 0; d < datasize_arr_count; d++){
        for (int b = 0; b < buffer_size_arr_count; b++){
        // reset the timer counts
        counter_transfer = 0;
        counter_all = 0;
        size_t test_data_size = datasize_arr[d];
        size_t bufsize = buffer_size_arr[b];
        if (mode != LATENCY)
            test_data_size = (d + 1) * S_512KB;
        for (int rep = 1; rep <= MAX_REP; rep++){
            counter_transfer += (*ipc_func[ipc_func_index])(test_data_size, bufsize, timer_func, mode);
        }
        reportable_avg_transfer =  counter_transfer / MAX_REP / mode;
        printf("%s,%ld,%ld,%lu,\n",
                    ipc_func_name[ipc_func_index],
                    test_data_size,
                    bufsize,
                    reportable_avg_transfer
                    );
        fflush(stdout);
        }
    }
}



int main(int argc, char **argv){
    //check_current_timer_readings();
    if (argc != 4){
        printf("usage: ./measure_[name] [ipc func] [timer opt] [mode opt] [buf size]\n");
        printf("ipc func: 1 = Pipe; 2 = Socket; 3 = Shared Mem\n");
        printf("timer: 1 = RDTSC; 2 = Clock_Mono; 3 = TimeOfDay\n");
        printf("mode: 1 = Throughput; 2 = Latency;\n");
        return 0;
    }
    int ipc_func_index = (int)(argv[1][0] - '1');
    int timer_index = (int)(argv[2][0] - '1');
    int mode = (int)(argv[3][0] - '0');
    
    perform_measurement(ipc_func_index, mode, available_timer_funcs[timer_index]);
    return 0;
}
