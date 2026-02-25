# Define the C compiler to use
CC=gcc

#Define any compile-time flags
CFLAGS=-Wall -g

#Define the target executable name
TARGET=FCFS

#'all' is the default target
all: $(TARGET)

#the target will e rebuilt when "make" is run
$(TARGET): $(TARGET).C
		$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).C