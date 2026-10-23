CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic

all: test

test: CIntN.o test.o
	$(CXX) $(CXXFLAGS) CIntN.o test.o -o test

CIntN.o: CIntN.cpp CIntN.h
	$(CXX) $(CXXFLAGS) -c CIntN.cpp

test.o: test.cpp CIntN.h
	$(CXX) $(CXXFLAGS) -c test.cpp

clean:
	rm -f *.o test

.PHONY: all clean
