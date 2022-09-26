./measure_basava 1 1 2  > ./results_vary_buf/basava__Pipe__RDTSC__LAT.csv
echo 'done'
./measure_basava 1 2 2  > ./results_vary_buf/basava__Pipe__ClockMono__LAT.csv
echo 'done'
./measure_basava 1 3 2  > ./results_vary_buf/basava__Pipe__TimeOfDay__LAT.csv
echo 'done'
./measure_basava 2 1 2  > ./results_vary_buf/basava__Socket__RDTSC__LAT.csv
echo 'done'
./measure_basava 2 2 2  > ./results_vary_buf/basava__Socket__ClockMono__LAT.csv
echo 'done'
./measure_basava 2 3 2  > ./results_vary_buf/basava__Socket__TimeOfDay__LAT.csv
echo 'done'
./measure_basava 3 1 2  > ./results_vary_buf/basava__SharedMem__RDTSC__LAT.csv
echo 'done'
./measure_basava 3 2 2  > ./results_vary_buf/basava__SharedMem__ClockMono__LAT.csv
echo 'done'
./measure_basava 3 3 2  > ./results_vary_buf/basava__SharedMem__TimeOfDay__LAT.csv
echo 'done'
