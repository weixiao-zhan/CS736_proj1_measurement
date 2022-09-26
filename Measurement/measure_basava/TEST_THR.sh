./measure_basava 1 1 1 5000 > ./results_thr/basava__Pipe__RDTSC__THR__5k.csv
echo 'done'
./measure_basava 1 2 1 5000 > ./results_thr/basava__Pipe__ClockMono__THR__5k.csv
echo 'done'
./measure_basava 1 3 1 5000 > ./results_thr/basava__Pipe__TimeOfDay__THR__5k.csv
echo 'done'
./measure_basava 2 1 1 5000 > ./results_thr/basava__Socket__RDTSC__THR__5k.csv
echo 'done'
./measure_basava 2 2 1 5000 > ./results_thr/basava__Socket__ClockMono__THR__5k.csv
echo 'done'
./measure_basava 2 3 1 5000 > ./results_thr/basava__Socket__TimeOfDay__THR__5k.csv
echo 'done'
./measure_basava 3 1 1 5000 > ./results_thr/basava__SharedMem__RDTSC__THR__5k.csv
echo 'done'
./measure_basava 3 2 1 5000 > ./results_thr/basava__SharedMem__ClockMono__THR__5k.csv
echo 'done'
./measure_basava 3 3 1 5000 > ./results_thr/basava__SharedMem__TimeOfDay__THR__5k.csv
echo 'done'
