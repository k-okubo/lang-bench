TARGET := main
OBJS := main.o
CC := g++
CFLAGS := -Wall -O3

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

.SUFFIXES: .cpp
.cpp.o:
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: run
run: $(TARGET)
	cat ../input | TIME="user:%U sys:%S" /usr/bin/time ./$(TARGET)
