CC = g++
FLAGS = -O3 -Wall -Werror -std=c++14 -pedantic -g# -march=sandybridge

HEADER = DataAcc.h

.PHONY: all clean

all: testDataAcc

testDataAcc: testDataAcc.o DataAcc.o
	$(CC) $(FLAGS) -o $@ $^

%.o: %.cpp $(HEADER)
	$(CC) $(FLAGS) -o $@ -c $<

clean:
	rm -f *.o testDataAcc
