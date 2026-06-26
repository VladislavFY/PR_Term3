CC = g++
CFLAGS = -Wall -Wextra -std=c++17
TARGET = task16
OBJECTS = main.o StringBuffer.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

main.o: main.cpp StringBuffer.hpp
	$(CC) $(CFLAGS) -c main.cpp

StringBuffer.o: StringBuffer.cpp StringBuffer.hpp
	$(CC) $(CFLAGS) -c StringBuffer.cpp

clean:
	rm -f $(OBJECTS) $(TARGET)
