OUTPUT=baseline_perf.csv

export KMP_AFFINITY=granularity=fine,compact,1,28

iters=100
M1=$1
N1=$2
K1=$3
NUM_THREADS=$4
config=${M1}_${N1}_${K1}_${NUM_THREADS}
export OMP_NUM_THREADS=${NUM_THREADS}

(cd .. && make clean && make  MACROFLAGS="-DBASELINE -DM1=$M1 -DN1=$N1 -DK1=$K1 -DNUM_ITERS=$iters")

../matmul &> run_output
GFLOPS=`cat run_output |  grep Real_GFLOPS |  cut -d= -f2`
echo ${config},${GFLOPS}  >> ${OUTPUT}

