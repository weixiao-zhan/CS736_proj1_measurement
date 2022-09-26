# Measurements



## File structure

the interface and measurement's main program is in the current directory.

each folder contains measurement tests and results for each implementation.

the results folder in each implementation folder contains all the CSV results.

## How to measure
you can make and run the ./measure_[name] executable as following to see in console output:
```
usage: ./measure_[name] [ipc func] [timer opt] [mode opt] [buf size]
ipc func: 1 = Pipe; 2 = Socket; 3 = Shared Mem
timer: 1 = RDTSC; 2 = Clock_Mono; 3 = TimeOfDay
mode: 1 = Throughput; 2 = Latency;
buf size: buffer size. Max 1000000(not really in some cases. Need to change this)
```

Or run TEST.sh to generate all the csv files in results folder. I chose the more primitive way to make sure it works.

## Data explanation

each file is labeled by  Implementer__Which Mechanism used __ which timer it used __ is it for latency(LAT) or Throughput __ which buffer_size is used

within each file we have each of the following column:

mechanism, data size, buffer/package size, how many reps for avg, Transfer time, Overall time

-mechanism: machanism used
-data size: how much data in bytes is each transfer
-buffer/package size: buffer size
-how many reps for avg: I repeat the communication many times and summed the time up. then I get the average at different point along the way. Hope is to get the values converge to a more accurate measurement.
-Transfer time: time taken to just do transfer (from first write to last read)
-Overall time: time taken from calling the function as a whole, which included all the preparation

Time uint is different, which I intend to convert in post-process instead of in the program. For RDTSC, it is still cycles. For CLockMono, it is nano secs, and For TimeOfDay it is micro seconds.


## Modifications

I modified and inserted timer, which I think it works????

I am at the point that I cannot easily find errors in the stuffs I wrote, so I need help reviewing the code.

For socket.c and pipe.c in Weixiao's implementation, I was going to put before write helper, but that would count all the memset and malloc operations so I plugged it into the helper instead.

Added free, fd closing, and setsockopt to make it work during measuring.

Still need to make naming better and clean things up


## Next Steps
I will start analyzing the data I have. Also the data looks.......kinda counter-intuitive to me.......which is another reason why I need reviews and comments.

I also still need Basava's TCP/IP implementation for socket, which is also why I commented out that part in the TEST.sh
