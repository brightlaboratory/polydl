CC=icc -std=c99
CFLAGS=-O3 -I . -I../libxsmm/include
LDFLAGS=-L../libxsmm/lib -lxsmm -lxsmmext -lpthread -lm
MACROFLAGS=

default: lstm


lstm: lstm.c ../libxsmm/include/libxsmm.h
	$(CC) $(CFLAGS) $(MACROFLAGS) lstm.c $(LDFLAGS) -o lstm

clean: 
	rm -rf lstm

