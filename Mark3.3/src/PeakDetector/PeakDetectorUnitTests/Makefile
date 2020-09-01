CC = g++
CFLAGS  = -Wall
CLASSLIB = PeakDetector
LIBPATH = ../$(CLASSLIB).cpp
TARGET = $(CLASSLIB)UnitTests


all: $(TARGET)

$(TARGET): $(TARGET).cpp $(LIBPATH)
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp $(LIBPATH)

clean:	
	rm -f $(TARGET)

