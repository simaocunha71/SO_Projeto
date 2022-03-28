CC = gcc
CFLAGS = -Wall -g

all: server client nop gcompress gdecompress bcompress bdecompress encrypt decrypt

server: sdstored

client: sdstore

sdstored: src/sdstored.c
			@echo "> Compiling server..."
			$(CC) $(CFLAGS) src/sdstored.c -o sdstored

sdstore: src/sdstore.c
			@echo "> Compiling client..."
			$(CC) $(CFLAGS) src/sdstore.c -o sdstore

gcompress: bin/gcompress.c
			@echo "> Compiling gcompress..."
			$(CC) $(CFLAGS) bin/gcompress.c -o obj/gcompress

gdecompress: bin/gdecompress.c
			@echo "> Compiling gdecompress..."
			$(CC) $(CFLAGS) bin/gdecompress.c -o obj/gdecompress

bcompress: bin/bcompress.c
			@echo "> Compiling bcompress..."
			$(CC) $(CFLAGS) bin/bcompress.c -o obj/bcompress

bdecompress: bin/bdecompress.c
			@echo "> Compiling bdecompress..."
			$(CC) $(CFLAGS) bin/bdecompress.c -o obj/bdecompress

encrypt: bin/encrypt.c
			@echo "> Compiling encrypt..."
			$(CC) $(CFLAGS) bin/encrypt.c -o obj/encrypt

decrypt: bin/decrypt.c
			@echo "> Compiling decrypt..."
			$(CC) $(CFLAGS) bin/decrypt.c -o obj/decrypt

nop: bin/nop.c
			@echo "> Compiling nop..."
			$(CC) $(CFLAGS) bin/nop.c -o obj/nop

clean:
	@echo "> Removing object files..."
	rm -f obj/*
