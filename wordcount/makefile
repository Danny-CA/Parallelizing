CC = gcc
CFLAGS = -O2 -fopenmp
TARGET = wordcount

all: $(TARGET)

$(TARGET): main.o wordcount.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o wordcount.o

main.o: main.c wordcount.h
	$(CC) $(CFLAGS) -c main.c

wordcount.o: wordcount.c wordcount.h uthash.h
	$(CC) $(CFLAGS) -c wordcount.c

clean:
	rm -f *.o $(TARGET) result.txt
