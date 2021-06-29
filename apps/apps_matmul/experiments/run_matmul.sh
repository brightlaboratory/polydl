
set -x
export KMP_AFFINITY=granularity=fine,compact,1,28
export LD_LIBRARY_PATH=/nfs_home/stavarag/work/software/barvinok/barvinok-0.41.2_install/lib:/nfs_home/stavarag/work/software/barvinok/isl_install/lib:$LD_LIBRARY_PATH

export LD_LIBRARY_PATH=/nfs_home/stavarag/work/software/barvinok/barvinok-0.41.2_install/lib:/nfs_home/stavarag/work/software/barvinok/isl_install/lib:$LD_LIBRARY_PATH


OUT=poly_perf.csv

check_correctness=1
PERF_DIR=perf_data
TEMP=temp
DATATYPESIZE=4

mkdir ${PERF_DIR}
mkdir ${TEMP}

iters=$1
M1=$2
N1=$3
K1=$4
M2_Tile=$5
N2_Tile=$6
K2_Tile=$7
M1_Tile=$8
N1_Tile=$9
K1_Tile=${10}
file=${11}
PARALLEL_LOOP=${12}
NUM_THREADS=${13}

echo iters=$iters M1=$M1 N1=$N1 K1=$K1 M2_Tile=$M2_Tile N2_Tile=$N2_Tile K2_Tile=$K2_Tile M1_Tile=$M1_Tile N1_Tile=$N1_Tile K1_Tile=$K1_Tile file=$file

config=${iters}_${M1}_${N1}_${K1}__${M2_Tile}_${N2_Tile}_${K2_Tile}_${M1_Tile}_${N1_Tile}_${K1_Tile}_${PARALLEL_LOOP}_${NUM_THREADS}
echo config: $config

CONFIG_OUT=${PERF_DIR}/${file}_${M1}_${N1}_${K1}_${OUT}
META_CONFIG_OUT=${PERF_DIR}/meta_${file}_${M1}_${N1}_${K1}_${OUT}
#rm ${CONFIG_OUT}
#rm ${META_CONFIG_OUT}

export OMP_NUM_THREADS=${NUM_THREADS} #FIXME

WORKFILE=$TEMP/temp.c
echo WORKFILE: $WORKFILE
rm $WORKFILE

echo "#define M1 $M1" >> $WORKFILE
echo "#define N1 $N1" >> $WORKFILE
echo "#define K1 $K1" >> $WORKFILE

echo "#define M2_Tile ${M2_Tile}" >> $WORKFILE
echo "#define N2_Tile ${N2_Tile}" >> $WORKFILE
echo "#define K2_Tile ${K2_Tile}" >> $WORKFILE

echo "#define M1_Tile ${M1_Tile}" >> $WORKFILE
echo "#define N1_Tile ${N1_Tile}" >> $WORKFILE
echo "#define K1_Tile ${K1_Tile}" >> $WORKFILE

cat ../versions/$file >> $WORKFILE

EXPERIMENTS_DIR=$PWD
(cd .. && make clean && make version_file=$EXPERIMENTS_DIR/$WORKFILE MACROFLAGS="-DM1=$M1 -DN1=$N1 -DK1=$K1 -DNUM_ITERS=$iters -DM2_Tile=${M2_Tile} -DN2_Tile=${N2_Tile} -DK2_Tile=${K2_Tile} -DM1_Tile=${M1_Tile} -DN1_Tile=${N1_Tile} -DK1_Tile=${K1_Tile} -DPARALLEL_${PARALLEL_LOOP}")

../matmul &> run_output
GFLOPS=`cat run_output |  grep Real_GFLOPS |  cut -d= -f2`
ERROR=`cat run_output | grep "inf-norm of comp. abs. error" | cut -d: -f 2`
echo GFLOPS: $GFLOPS
echo ERROR: $ERROR


CACHE_CONFIG='L1 32768 L2 1048576 L3 40370176'
DATATYPESIZE=4
NUM_PROCS=${NUM_THREADS}$
output_file=${EXPERIMENTS_DIR}/${WORKFILE}_ws_stats.csv

#../../../data_reuse_analyzer/polyscientist --input $EXPERIMENTS_DIR/$WORKFILE --parameters 'dummy : 50 ' --cachesizes "${CACHE_CONFIG}" --datatypesize $DATATYPESIZE --minout

time ../../../data_reuse_analyzer/polyscientist --input $EXPERIMENTS_DIR/$WORKFILE --parameters 'dummy : 50 ' --cachesizes "${CACHE_CONFIG}" --datatypesize $DATATYPESIZE --minout --parallel_loops ${PARALLEL_LOOP} --numprocs ${NUM_PROCS} --sharedcaches L3 

{ echo -n "${config},${GFLOPS}," ;  cat ${output_file} ; }  >> ${CONFIG_OUT}
echo  "${config},${ERROR}" >> ${META_CONFIG_OUT}


