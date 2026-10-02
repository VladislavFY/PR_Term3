all: CIntN.o test.o
	g++ CIntN.o test.o -o test && rm -f *.o

CIntN.o: CIntN.cpp CIntN.h
	g++ -c CIntN.cpp

test.o: test.cpp CIntN.h
	g++ -c test.cpp

clean:
	rm -f *.o test
