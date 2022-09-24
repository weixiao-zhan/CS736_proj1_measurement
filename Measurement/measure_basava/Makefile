measure_basava: measure.o comm.o
	g++ -o measure_basava measure.o comm.o

measure.o: ../measure.c ../interface.h
	yes | cp ../measure.c ../measure.cc
	g++ -c ../measure.cc
	yes | rm ../measure.cc

comm.o: comm.cc
	g++ -c comm.cc
