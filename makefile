CC = gcc
CFLAGS = -Wall

all: 
	$(CC) -o stegobmp main.c parser.c $(CFLAGS)

clean: 
	rm f *.o