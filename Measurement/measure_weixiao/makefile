measure_weixiao: measure.o pipe.o socket.o mem_share.o helper.o
	gcc -o measure_weixiao measure.o pipe.o socket.o mem_share.o helper.o

measure.o: ../measure.c ../interface.h
	gcc -c ../measure.c

pipe.o: pipe.c
	gcc -O3 -c pipe.c

socket.o: socket.c
	gcc -O3 -c socket.c -lpthread -lm -lrt

mem_share.o: mem_share.c
	gcc -O3 -c mem_share.c -lpthread -lm -lrt

helper.o: helper.c
	gcc -O3 -c helper.c

