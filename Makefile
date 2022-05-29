CC = gcc
CFLAGS = -Wall -g

all: server client nop gcompress gdecompress bcompress bdecompress encrypt decrypt

server: sdstored

client: sdstore

sdstored: src/sdstored.c
			@echo "> Compiling server..."
			$(CC) $(CFLAGS) src/sdstored.c -o bin/sdstored

sdstore: src/sdstore.c
			@echo "> Compiling client..."
			$(CC) $(CFLAGS) src/sdstore.c -o bin/sdstore

gcompress: bin/SDStore-transf/gcompress.c
			@echo "> Compiling gcompress..."
			$(CC) $(CFLAGS) bin/SDStore-transf/gcompress.c -o obj/gcompress

gdecompress: bin/SDStore-transf/gdecompress.c
			@echo "> Compiling gdecompress..."
			$(CC) $(CFLAGS) bin/SDStore-transf/gdecompress.c -o obj/gdecompress

bcompress: bin/SDStore-transf/bcompress.c
			@echo "> Compiling bcompress..."
			$(CC) $(CFLAGS) bin/SDStore-transf/bcompress.c -o obj/bcompress

bdecompress: bin/SDStore-transf/bdecompress.c
			@echo "> Compiling bdecompress..."
			$(CC) $(CFLAGS) bin/SDStore-transf/bdecompress.c -o obj/bdecompress

encrypt: bin/SDStore-transf/encrypt.c
			@echo "> Compiling encrypt..."
			$(CC) $(CFLAGS) bin/SDStore-transf/encrypt.c -o obj/encrypt

decrypt: bin/SDStore-transf/decrypt.c
			@echo "> Compiling decrypt..."
			$(CC) $(CFLAGS) bin/SDStore-transf/decrypt.c -o obj/decrypt

nop: bin/SDStore-transf/nop.c
			@echo "> Compiling nop..."
			$(CC) $(CFLAGS) bin/SDStore-transf/nop.c -o obj/nop

clean:
	@echo "> Removing object files..."
	rm -f obj/* bin/sdstore bin/sdstored
