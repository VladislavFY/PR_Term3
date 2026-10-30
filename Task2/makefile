all: CIntN.o test.o
	g++ CIntN.o test.o && rm *.o

CIntN.o: CIntN.cpp CIntN.h
	g++ -c CIntN.cpp

test.o: test.cpp CIntN.h
	g++ -c test.cpp

