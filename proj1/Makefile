CC = gcc

# flags
CFLAGS = -Wall -g

#exe name
TARGET = huffandpuff

# object files
OBJS = huffandpuff.o minheap.o

# default target
all: $(TARGET)

# link object files to create executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# compile source files to object files
huffman.o: huffandpuff.c minheap.h
	$(CC) $(CFLAGS) -c huffandpuff.c

minheap.o: minheap.c minheap.h
	$(CC) $(CFLAGS) -c minheap.c

# clean up
clean:
	rm -f $(OBJS) $(TARGET)
