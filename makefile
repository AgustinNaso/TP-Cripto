CC = gcc
CFLAGS = -Wall -Wuninitialized -lcrypto

all: 
	$(CC) -o stegobmp main.c embed.c encrypt.c utils.c decrypt.c parser.c ./extractHandler/extract.c ./extractHandler/helper.c fileHandler.c $(CFLAGS)

clean: 
	rm f *.o