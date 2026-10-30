
all: StringContainer.o test.o
	g++ StringContainer.o test.o -o && rm *.o

StringContainer.o: StringContainer.cpp StringContainer.h
	g++ -c StringContainer.cpp

test.o: test.cpp StringContainer.h
	g++ -c test.cpp
