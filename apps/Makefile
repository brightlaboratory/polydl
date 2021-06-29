CC=icc
CFLAGS=-O3 -I . -I./libxsmm/include -qopenmp -fno-alias -xCOMMON-AVX512 -mkl=sequential
LDFLAGS=-L./libxsmm/lib -lxsmm -lxsmmext -lpthread
MACROFLAGS=

default: conv2d

./libxsmm/include/libxsmm.h:
	rm -rf libxsmm/
	git clone https://github.com/hfp/libxsmm.git
	$(MAKE) realclean -C libxsmm
	$(MAKE) AVX=3 BLAS=0 -C libxsmm

conv2d: conv2d.c ./libxsmm/include/libxsmm.h
	$(CC) $(CFLAGS) $(MACROFLAGS) gemm.c conv2d.c $(LDFLAGS) -o conv2d

clean: 
	rm -rf conv2d

