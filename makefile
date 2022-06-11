CC = gcc
CFLAGS = -Wall -Wuninitialized

all: 
	$(CC) -o stegobmp main.c parser.c ./extractHandler/extract.c ./extractHandler/helper.c fileHandler.c $(CFLAGS)

clean: 
	rm f *.o