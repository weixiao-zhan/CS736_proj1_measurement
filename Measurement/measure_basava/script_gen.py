ipc = [(1,"Pipe"),(2,"Socket"),(3,"SharedMem")]
timer = [(1,"RDTSC"),(2,"ClockMono"),(3,"TimeOfDay")]
test_opt = [(1,"THR"), (2,"LAT")]
buf_choice = [(1000, "1k"),(5000, "5k"),(10000, "10k")]


for i in ipc:
    for t in timer:
        for o in test_opt:
            for b in buf_choice:
                print("./measure_basava "+str(i[0])+" "+str(t[0])+" "+str(o[0])+" "+str(b[0])+" > ./results/basava__"+i[1]+"__"+t[1]+"__"+o[1]+"__"+b[1]+".csv")
                print("echo 'done'")
