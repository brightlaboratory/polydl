TVM_OUTPUT_DIR=tvm_output
mkdir ${TVM_OUTPUT_DIR}
OUT=TVM_perf.csv
#rm ${OUT}
SCRIPT=/nfs_home/stavarag/work/test/polyscientist/TVM/simplified_tune_relay_x86_custom_generic.py #FIXME

set -x
source /nfs_home/stavarag/work/software/Python-3.7.1_install/venv/bin/activate


#Default values.
iters=1
ifw=56
ifh=56
nIfm=64
nOfm=256
kw=1
kh=1
pad_w=0
pad_h=0
stride=1


iters=$1
ifw=$2
ifh=$3
nIfm=$4
nOfm=$5
kw=$6
kh=$7
pad_w=$8
pad_h=$9
stride=${10}
config_num=${11}

config="${iters} ${ifw} ${ifh} ${nIfm} ${nOfm} ${kw} ${kh} ${pad_w} ${pad_h} ${stride}"

RUN_OUTPUT=${TVM_OUTPUT_DIR}/${config_num}_output.txt
python3 $SCRIPT $config &> ${RUN_OUTPUT}

GFLOPS=`cat ${RUN_OUTPUT} | grep "GFLOPS" |rev| cut -d "|" -f3|rev | cut -d " " -f10 | cut -d "/" -f2`

echo -n ${config_num},$GFLOPS >> ${OUT}
echo "" >> ${OUT}

