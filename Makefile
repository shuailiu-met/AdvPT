CC = g++
FLAGS = -O3 -Wall -Werror -std=c++14 -pedantic# -march=sandybridge

HEADER = General/DataAcc.h General/Unit.h Protoss/Protoss.h
RACES = Race.h Protoss/Protoss.h

.PHONY: all clean

all: simulate

simulate: simulate.o Race.o General/DataAcc.o Protoss/Protoss.o
	$(CC) $(FLAGS) -o $@ $^

Race.o: Race.cpp General/DataAcc.h General/Unit.h
	$(CC) $(FLAGS) -o $@ -c $<

Protoss/Protoss.o: Protoss/Protoss.cpp Protoss/Protoss.h General/Unit.h
	$(CC) $(FLAGS) -o $@ -c $<

DataAcc.o: General/DataAcc.cpp General/DataAcc.h General/Unit.h
	$(CC) $(FLAGS) -o $@ -c $<

simulate.o: simulate.cpp General/DataAcc.h $(RACES)
	$(CC) $(FLAGS) -o $@ -c $<

%.o: %.cpp $(HEADER)
	$(CC) $(FLAGS) -o $@ -c $<

clean:
	rm -f *.o General/*.o simulate Protoss/*.o
