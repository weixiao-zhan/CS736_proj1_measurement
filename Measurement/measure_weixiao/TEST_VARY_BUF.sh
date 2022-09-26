./measure_weixiao 1 1 2  > ./results_vary_buf/weixiao__Pipe__RDTSC__LAT.csv
echo 'done'
./measure_weixiao 1 2 2  > ./results_vary_buf/weixiao__Pipe__ClockMono__LAT.csv
echo 'done'
./measure_weixiao 1 3 2  > ./results_vary_buf/weixiao__Pipe__TimeOfDay__LAT.csv
echo 'done'
./measure_weixiao 2 1 2  > ./results_vary_buf/weixiao__Socket__RDTSC__LAT.csv
echo 'done'
./measure_weixiao 2 2 2  > ./results_vary_buf/weixiao__Socket__ClockMono__LAT.csv
echo 'done'
./measure_weixiao 2 3 2  > ./results_vary_buf/weixiao__Socket__TimeOfDay__LAT.csv
echo 'done'
./measure_weixiao 3 1 2  > ./results_vary_buf/weixiao__SharedMem__RDTSC__LAT.csv
echo 'done'
./measure_weixiao 3 2 2  > ./results_vary_buf/weixiao__SharedMem__ClockMono__LAT.csv
echo 'done'
./measure_weixiao 3 3 2  > ./results_vary_buf/weixiao__SharedMem__TimeOfDay__LAT.csv
echo 'done'
