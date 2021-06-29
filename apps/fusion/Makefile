CC=icc
CFLAGS=-O3 -I . -I../libxsmm/include -qopenmp -fno-alias -xCOMMON-AVX512 -mkl=sequential
LDFLAGS=-L../libxsmm/lib -lxsmm -lxsmmext -lpthread
MACROFLAGS=

default: conv2d_relu

../libxsmm/include/libxsmm.h:
	rm -rf ..libxsmm/
	git clone --branch 1.14 https://github.com/hfp/libxsmm.git
	$(MAKE) realclean -C ../libxsmm
	$(MAKE) AVX=3 BLAS=0 -C ../libxsmm

conv2d_relu: conv2d_relu.c ../libxsmm/include/libxsmm.h
	$(CC) $(CFLAGS) $(MACROFLAGS) conv2d_relu.c $(LDFLAGS) -o conv2d_relu

clean: 
	rm -rf conv2d_relu

