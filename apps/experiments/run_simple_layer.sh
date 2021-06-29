#set -x


compute_padding()
{

MAX_BLOCK=64
input=$1
output=$input
MIN_BLOCK=16

#Padding
for BLOCK in 16 32 64
do
        if [ `expr $input % $BLOCK` -eq 0 ]
        then
          break
        fi

        if [ $input -lt $BLOCK ]
        then
          let output=$BLOCK
          break
        else
          if [ $BLOCK -eq $MAX_BLOCK -a $input -gt $MAX_BLOCK ]
          then
                let output=( $input + $MIN_BLOCK -  $input % $MIN_BLOCK  )
          fi
        fi
done

echo $output
}

compute_GEMM_BLOCK()
{
  nIfm=$1
  nOfm=$2

  #echo In compute_GEMM_BLOCK. nIfm: $nIfm nOfm: $nOfm

  for BLOCK in 64 32 16
  do
        if [ `expr $nIfm % $BLOCK` -eq 0 -a `expr $nOfm % $BLOCK` -eq 0 ]
        then
          #echo $BLOCK divides them both
          echo $BLOCK
          break
        fi
  done
}


MB=28
export KMP_AFFINITY=granularity=fine,compact,1,28
export OMP_NUM_THREADS=$MB
LAYER_EXAMPLE_ICC_DIR=/nfs_home/stavarag/work/libxsmm/libxsmm_simple_icc_password-1234/libxsmm_simple_icc
LAYER_EXAMPLE_ICC=layer_example_icc


iters=$1
ifw=$2
ifh=$3
orig_nIfm=$4
orig_nOfm=$5
kw=$6
kh=$7
pad_w=$8
pad_h=$9
stride=${10}
config_num=${11}
TYPE=F
OUTPUT=layer_example_icc.output
DIR=simple_layer_perf
mkdir $DIR
COMP_OUT=compiler.csv
GEMM_OUT=gemm.csv

nIfm=$(compute_padding $orig_nIfm)
echo orig_nIfm: $orig_nIfm nIfm: $nIfm

nOfm=$(compute_padding $orig_nOfm)
echo orig_nOfm: $orig_nOfm  nOfm: $nOfm

GEMM_BLOCK=$(compute_GEMM_BLOCK $nIfm $nOfm)
echo GEMM_BLOCK: $GEMM_BLOCK

(cd ${LAYER_EXAMPLE_ICC_DIR} && make clean && make MACROFLAGS="-DCHANNEL_BLOCK=$GEMM_BLOCK")
${LAYER_EXAMPLE_ICC_DIR}/${LAYER_EXAMPLE_ICC} ${iters}   ${ifw}   ${ifh}  ${MB} ${nIfm}  ${nOfm} ${kw} ${kh} ${pad_w} ${pad_h} ${stride} ${TYPE} &> ${OUTPUT}

Orig_GFLOPS_comp=`cat $OUTPUT | grep GFLOPS_compiler | cut -d= -f2`
Orig_GFLOPS_gmm=`cat $OUTPUT | grep GFLOPS_gemm | cut -d= -f2`

GFLOPS_comp=$(echo "$Orig_GFLOPS_comp * $orig_nIfm * $orig_nOfm  /  $nIfm / $nOfm"|bc )
GFLOPS_gmm=$(echo "$Orig_GFLOPS_gmm * $orig_nIfm * $orig_nOfm  /  $nIfm / $nOfm"|bc )

echo ${config_num},${GFLOPS_comp} >> $DIR/${COMP_OUT}
echo ${config_num},${GFLOPS_gmm} >> $DIR/${GEMM_OUT}
