CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic

all: test

test: FileSystem.o test.o
	$(CXX) $(CXXFLAGS) FileSystem.o test.o -o test

FileSystem.o: FileSystem.cpp FileSystem.h
	$(CXX) $(CXXFLAGS) -c FileSystem.cpp

test.o: test.cpp FileSystem.h
	$(CXX) $(CXXFLAGS) -c test.cpp

clean:
	rm -f *.o test
