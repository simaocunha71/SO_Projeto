CC = gcc
CFLAGS = -Wall -g

all: nop gcompress gdecompress bcompress bdecompress encrypt decrypt

gcompress: gcompress.o

gcompress.o: src/gcompress.c

gdecompress: gdecompress.o

gdecompress.o: src/gdecompress.c

bcompress: bcompress.o

bcompress.o: src/bcompress.c

bdecompress: bdecompress.o

bdecompress.o: src/bdecompress.c

encrypt: encrypt.o

encrypt.o: src/encrypt.c

decrypt: decrypt.o

decrypt.o: src/decrypt.c

nop: nop.o

nop.o: src/nop.c

clean:
	rm -f *compress *decompress encrypt decrypt nop *.o
