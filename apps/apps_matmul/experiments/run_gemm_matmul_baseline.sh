OUTPUT=gemm_baseline_perf.csv
META_OUTPUT=meta_${OUTPUT}

iters=100
M1=$1
N1=$2
K1=$3
NUM_THREADS=$4
USELIBXSMM_GEMM=$5

config=${M1}_${N1}_${K1}_${NUM_THREADS}_${USELIBXSMM_GEMM}
export OMP_NUM_THREADS=${NUM_THREADS}

rm gemm_matmul
LIBXSMM_LIB=../../libxsmm/
icc -O3 -I${LIBXSMM_LIB}/include -fopenmp -DM1=$M1 -DN1=$N1 -DK1=$K1 -DNUM_ITERS=$iters -DUSELIBXSMM_GEMM=1 -L${LIBXSMM_LIB}/lib ../versions/gemm_matmul.c -o gemm_matmul -lxsmm -lxsmmext -ldl -lm

./gemm_matmul &> run_output
GFLOPS=`cat run_output |  grep Real_GFLOPS |  cut -d= -f2`
ERROR=`cat run_output | grep "inf-norm of comp. abs. error" | cut -d: -f 2`

echo ${config},${GFLOPS}  >> ${OUTPUT}
echo ${config},${ERROR} >> ${META_OUTPUT}

