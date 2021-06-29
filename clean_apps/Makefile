CC=gcc
CFLAGS=-O3 -I . -I./libxsmm/include -fopenmp -DUSE_LIBXSMM 
LDFLAGS=-L./libxsmm/lib -lxsmm -lxsmmext -ldl -lm 
MACROFLAGS=

default: conv2d

./libxsmm/include/libxsmm.h:
	rm -rf libxsmm/
	git clone https://github.com/hfp/libxsmm.git
	$(MAKE) realclean -C libxsmm
	$(MAKE) BLAS=0 -C libxsmm

conv2d: conv2d.c ./libxsmm/include/libxsmm.h
	$(CC) $(CFLAGS) $(MACROFLAGS) gemm.c $(version_file) conv2d.c $(LDFLAGS) -o conv2d

clean: 
	rm -rf conv2d

