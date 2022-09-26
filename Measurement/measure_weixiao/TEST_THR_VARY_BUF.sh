./measure_weixiao 1 1 1  > ./results_vary_buf/weixiao__Pipe__RDTSC__THR.csv
echo 'done'
./measure_weixiao 1 2 1  > ./results_vary_buf/weixiao__Pipe__ClockMono__THR.csv
echo 'done'
./measure_weixiao 1 3 1  > ./results_vary_buf/weixiao__Pipe__TimeOfDay__THR.csv
echo 'done'
./measure_weixiao 2 1 1  > ./results_vary_buf/weixiao__Socket__RDTSC__THR.csv
echo 'done'
./measure_weixiao 2 2 1  > ./results_vary_buf/weixiao__Socket__ClockMono__THR.csv
echo 'done'
./measure_weixiao 2 3 1  > ./results_vary_buf/weixiao__Socket__TimeOfDay__THR.csv
echo 'done'
./measure_weixiao 3 1 1  > ./results_vary_buf/weixiao__SharedMem__RDTSC__THR.csv
echo 'done'
./measure_weixiao 3 2 1  > ./results_vary_buf/weixiao__SharedMem__ClockMono__THR.csv
echo 'done'
./measure_weixiao 3 3 1  > ./results_vary_buf/weixiao__SharedMem__TimeOfDay__THR.csv
echo 'done'
