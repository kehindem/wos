CC = g++
CFLAGS  = -Wall
CLASSLIB = PeopleCounter
LIBPATH = ../$(CLASSLIB).cpp
TARGET = $(CLASSLIB)UnitTests
DEPENDENCIES = ../../PeakDetector/PeakDetector.cpp PeopleCounterTestClass.cpp

all: $(TARGET)

$(TARGET): $(TARGET).cpp $(LIBPATH)
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp $(LIBPATH) $(DEPENDENCIES)

clean:	
	rm -f $(TARGET)

