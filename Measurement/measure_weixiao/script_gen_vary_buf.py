ipc = [(1,"Pipe"),(2,"Socket"),(3,"SharedMem")]
timer = [(1,"RDTSC"),(2,"ClockMono"),(3,"TimeOfDay")]
test_opt = [(1,"THR")]


for i in ipc:
    for t in timer:
        for o in test_opt:
            print("./measure_weixiao "+str(i[0])+" "+str(t[0])+" "+str(o[0])+"  > ./results_vary_buf/weixiao__"+i[1]+"__"+t[1]+"__"+o[1]+".csv")
            print("echo 'done'")
