
 /nfs_home/stavarag/work/software/PLUTO/pluto-0.11.4/polycc pluto_matmul.c --tile --parallel -o matmul.par.c

LIBXSMM_LIB=../../libxsmm/
icc -O3 -I${LIBXSMM_LIB}/include -fopenmp -L${LIBXSMM_LIB}/lib matmul.par.c -o matmul_pluto -lxsmm -lxsmmext -ldl -lm
