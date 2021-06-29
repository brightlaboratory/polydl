#!/bin/bash
TVM_OUTPUT_DIR=tvm_output
mkdir ${TVM_OUTPUT_DIR}
OUT=TVM_perf.csv
#rm ${OUT}
SCRIPT=$PWD/autotvmMatmul.py

set -x

M=$1
N=$2
K=$3
config_num="${M}_${N}_${K}"
config="${M} ${N} ${K}"

RUN_OUTPUT=${TVM_OUTPUT_DIR}/${config_num}_output.txt
python3 $SCRIPT $config > ${RUN_OUTPUT}

GFLOPS=`cat ${RUN_OUTPUT} | grep "GFLOPS" | cut -d ":" -f3 | cut -f1 | cut -d "/" -f2`
#cat tvm_output/64_64_64_output.txt | grep "GFLOPS" | cut -d ":" -f3 | cut -d $'\t' -f1 | cut -d "/" -f2
GFLOPS=`echo $GFLOPS | sed 's/ /,/g'`
echo -n ${config_num},$GFLOPS >> ${OUT}
echo "" >> ${OUT}
