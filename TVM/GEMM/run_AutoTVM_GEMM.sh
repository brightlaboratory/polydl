set -x
M=$1
N=$2
K=$3
sizes="${M}_${N}_${K}"
INPUT_FILE="inputs_${sizes}"
sh create_tile_size_csv.sh $M $N $K $INPUT_FILE

source /nfs_home/stavarag/work/software/Python-3.7.1_install/venv/bin/activate
export KMP_AFFINITY=granularity=fine,compact,1,0
OUT=${INPUT_FILE}_perf.csv
TEMP=temp_$OUT
#rm ${OUT}

while IFS=, read -r bf1 bf2 factor M N K; do

    # echo "$bf1|$bf2|$factor|$M|$N|$K";
    config="${bf1}_${bf2}_${factor}_${M}_${N}_${K}"
    python3 AutoTVM_GEMM.py $bf1 $bf2 $factor $M $N $K &>$TEMP
    GFLOPS_OUT=`cat $TEMP | grep GFLOPS | cut -d" " -f3`;
    # echo $config;
    # echo $GFLOPS_OUT
    echo -n "${config},${GFLOPS_OUT}," >> ${OUT}
    echo >> ${OUT}

done < $INPUT_FILE

