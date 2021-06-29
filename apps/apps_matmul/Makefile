CC=icc
CFLAGS=-O3 -I . -I../libxsmm/include -fopenmp -DUSE_LIBXSMM 
LDFLAGS=-L../libxsmm/lib -lxsmm -lxsmmext -ldl -lm 
MACROFLAGS=

default: matmul

../libxsmm/include/libxsmm.h:
	rm -rf ../libxsmm/
	git clone --branch 1.14 https://github.com/hfp/libxsmm.git ../libxsmm
	$(MAKE) realclean -C ../libxsmm
	$(MAKE) AVX=3 BLAS=0 CXX=icc CC=icc FC=ifort -C ../libxsmm

matmul: matmul.c ../libxsmm/include/libxsmm.h
	$(CC) $(CFLAGS) $(MACROFLAGS) $(version_file) matmul.c $(LDFLAGS) -o matmul

clean: 
	rm -rf matmul
