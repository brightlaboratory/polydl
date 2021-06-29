set -x

OUT=perf.csv

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

images=28
    
export OMP_NUM_THREADS=${images}
./conv2d ${iters} ${ifw} ${ifh} ${nIfm} ${nOfm} ${kw} ${kh} ${pad_w} ${pad_h} ${stride} ${images} &> run_output
GFLOPS=`cat run_output |  grep GFLOPS |  cut -d= -f2`
echo "${config_num},${GFLOPS}"  >> ${OUT}
