
set -x
#export KMP_AFFINITY=granularity=fine,compact,1,28
export LD_LIBRARY_PATH=/nfs_home/stavarag/work/software/barvinok/barvinok-0.41.2_install/lib:/nfs_home/stavarag/work/software/barvinok/isl_install/lib:$LD_LIBRARY_PATH

#export LD_LIBRARY_PATH=/nfs_home/stavarag/work/software/barvinok/barvinok-0.41.2_install/lib:/nfs_home/stavarag/work/software/barvinok/isl_install/lib:$LD_LIBRARY_PATH


OUT=pluto_perf.csv

check_correctness=1
PERF_DIR=perf_data
TEMP=temp
DATATYPESIZE=4
file=pluto_matmul.c

mkdir ${PERF_DIR}
mkdir ${TEMP}

iters=$1
M1=$2
N1=$3
K1=$4
M1_Tile=$5
N1_Tile=$6
K1_Tile=${7}
NUM_THREADS=${8}

echo iters=$iters M1=$M1 N1=$N1 K1=$K1 M1_Tile=$M1_Tile N1_Tile=$N1_Tile K1_Tile=$K1_Tile NUM_THREADS=${NUM_THREADS}

config=${iters}_${M1}_${N1}_${K1}__${M1_Tile}_${N1_Tile}_${K1_Tile}_${NUM_THREADS}
echo config: $config

CONFIG_OUT=${PERF_DIR}/${M1}_${N1}_${K1}_${OUT}
META_CONFIG_OUT=${PERF_DIR}/meta_${M1}_${N1}_${K1}_${OUT}
#rm ${CONFIG_OUT}
#rm ${META_CONFIG_OUT}

export OMP_NUM_THREADS=${NUM_THREADS} #FIXME

WORKFILE=$TEMP/temp.c
TILEFILE=$TEMP/tile.sizes
echo WORKFILE: $WORKFILE
rm $WORKFILE
rm $TILEFILE

echo "#define M1 $M1" >> $WORKFILE
echo "#define N1 $N1" >> $WORKFILE
echo "#define K1 $K1" >> $WORKFILE

echo "#define M1_Tile ${M1_Tile}" >> $WORKFILE
echo "#define N1_Tile ${N1_Tile}" >> $WORKFILE
echo "#define K1_Tile ${K1_Tile}" >> $WORKFILE

cat $file >> $WORKFILE

echo ${M1_Tile} >> $TILEFILE
echo ${N1_Tile} >> $TILEFILE
echo ${K1_Tile} >> $TILEFILE
cp $TILEFILE .

EXPERIMENTS_DIR=$PWD

rm matmul.par.c
 /nfs_home/stavarag/work/software/PLUTO/pluto-0.11.4/polycc $WORKFILE --tile --parallel -o matmul.par.c

rm ./matmul_pluto
LIBXSMM_LIB=../../libxsmm/
icc -O3 -I${LIBXSMM_LIB}/include -fopenmp -DM1=$M1 -DN1=$N1 -DK1=$K1 -DNUM_ITERS=$iters -DM1_Tile=${M1_Tile} -DN1_Tile=${N1_Tile} -DK1_Tile=${K1_Tile} -L${LIBXSMM_LIB}/lib matmul.par.c -o matmul_pluto -lxsmm -lxsmmext -ldl -lm


./matmul_pluto &> run_output

GFLOPS=`cat run_output |  grep Real_GFLOPS |  cut -d= -f2`
ERROR=`cat run_output | grep "inf-norm of comp. abs. error" | cut -d: -f 2`
echo GFLOPS: $GFLOPS
echo ERROR: $ERROR

echo "${config},${GFLOPS}" >> ${CONFIG_OUT}
echo "${config},${ERROR}" >> ${META_CONFIG_OUT}


