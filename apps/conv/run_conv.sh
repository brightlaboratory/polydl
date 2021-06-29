#!/usr/bin/env bash
# set -x
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

export KMP_AFFINITY=granularity=fine,compact,1,28

PERF_DIR=perf_data
mkdir ${PERF_DIR}
OUTPUT=poly_perf.csv
GEMM_VERSIONS='0 3'
images=1

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

let ofh="($ifh + 2 * $pad_h - $kh)/$stride + 1"
let ofw="($ifw + 2 * $pad_w - $kw)/$stride + 1"
ofhp=${ofh}
ofwp=${ofw}
arg_images=1
ifhp=${ifh}
ifwp=${ifw}


nIfm=$(compute_padding $orig_nIfm)
echo orig_nIfm: $orig_nIfm nIfm: $nIfm

nOfm=$(compute_padding $orig_nOfm)
echo orig_nOfm: $orig_nOfm  nOfm: $nOfm

GEMM_BLOCK=$(compute_GEMM_BLOCK $nIfm $nOfm)
echo GEMM_BLOCK: $GEMM_BLOCK
let t_ofm_tile_max="$nOfm/$GEMM_BLOCK"
let t_ifm_tile_max="$nIfm/$GEMM_BLOCK"

echo t_ofm_tile_max: $t_ofm_tile_max
echo t_ifm_tile_max: $t_ifm_tile_max


Step_M=4
Step_N=32
Step_K=4


for images in 28 #1 28
	do

		export OMP_NUM_THREADS=${images}


      for version in $GEMM_VERSIONS #FIXME
        do
        if [ $version -eq 0 ]
          then
            if [ `expr $nIfm % $GEMM_BLOCK` -eq 0 -a `expr $nOfm % $GEMM_BLOCK` -eq 0 ]
                then

              for (( T_oi=${ofw}; T_oi >= 1; T_oi=T_oi/2 ))
              do
              if [ `expr $ofw % $T_oi` -eq 0 ] 
              then
              for (( T_oj=${ofh}; T_oj >= 1; T_oj=T_oj/2 ))
              do
              if [ `expr $ofh % $T_oj` -eq 0 ] 
              then
              for (( T_ifm_tile=1; T_ifm_tile<= $t_ifm_tile_max; T_ifm_tile=T_ifm_tile*2 ))
              do
              if [ `expr $nIfm % $T_ifm_tile` -eq 0 ]
              then
                for (( T_ofm_tile=1; T_ofm_tile<= $t_ofm_tile_max; T_ofm_tile=T_ofm_tile*2 ))
                do
                if [ `expr $nOfm % $T_ofm_tile` -eq 0 ]
                then

                        for (( Step_M_i=1; Step_M_i<=$Step_M; Step_M_i=Step_M_i*4))
                        do  
                        if [ `expr $Step_M % ${Step_M_i}` -eq 0 ]
                        then

                        for (( Step_N_j=16; Step_N_j<=$Step_N; Step_N_j=Step_N_j*2))
                        do  
                            if [ `expr $Step_N % ${Step_N_j}` -eq 0 ]
                            then

                            for (( Step_K_k=1; Step_K_k<=$Step_K; Step_K_k=Step_K_k*4))
                            do  
                                if [ `expr $Step_K % ${Step_K_k}` -eq 0 ]
                                then
                                        python microkernel_codeGenerator.py ${Step_M_i} ${Step_N_j} ${Step_K_k} 0 0 0
                                        make clean && make MACROFLAGS="-DSTRIDE_H=$stride -DSTRIDE_W=$stride -DT_oi=$T_oi -DT_oj=$T_oj -DT_ifm_tile=$T_ifm_tile -DT_ofm_tile=$T_ofm_tile -DGEMM_BLOCK=${GEMM_BLOCK}"
                                            # do something
                                        echo  $T_oi " " $T_oj " " $T_ifm_tile " " $T_ofm_tile
                                        ./conv2d ${iters} ${ifw} ${ifh} ${nIfm} ${nOfm} ${kw} ${kh} ${pad_w} ${pad_h} ${stride} ${images} ${version} ${check_correctness} &> run_output
                                        GFLOPS=`cat run_output | grep Real_GFLOPS | cut -d"=" -f 2`
                                        ERROR=`cat run_output | grep "inf-norm of comp. abs. error" | cut -d: -f 2`
                                        RELERROR=`cat run_output | grep "inf-norm of comp. rel. error" | cut -d: -f 2`
                                        echo "GFLOPS="${GFLOPS}
                                        echo  "${version}_${T_oi}_${T_oj}_${T_ifm_tile}_${T_ofm_tile}_${GEMM_BLOCK},${Step_M_i}_${Step_N_j}_${Step_K_k},${GFLOPS},${ERROR},${RELERROR}" >> ${PERF_DIR}/${images}_${config_num}_${OUTPUT}
                                        # exit

                                    fi
                                done
                                fi
                            done
                        fi
                        done
                  fi
                  done
                  fi
                  done
                  fi
                  done
                  fi
                  done
            fi
        else
          if [ `expr $nIfm % $GEMM_BLOCK` -eq 0 -a `expr $nOfm % $GEMM_BLOCK` -eq 0 ]
                then

                  for (( Step_M_i=1; Step_M_i<=$Step_M; Step_M_i=Step_M_i*4))
                  do  
                  if [ `expr $Step_M % ${Step_M_i}` -eq 0 ]
                  then

                  for (( Step_N_j=16; Step_N_j<=$Step_N; Step_N_j=Step_N_j*2))
                  do  
                      if [ `expr $Step_N % ${Step_N_j}` -eq 0 ]
                      then

                      for (( Step_K_k=1; Step_K_k<=$Step_K; Step_K_k=Step_K_k*4))
                      do  
                          if [ `expr $Step_K % ${Step_K_k}` -eq 0 ]
                          then
                                  python microkernel_codeGenerator.py ${Step_M_i} ${Step_N_j} ${Step_K_k} 0 0 0
                                  make clean && make MACROFLAGS="-DSTRIDE_H=$stride -DSTRIDE_W=$stride -DGEMM_BLOCK=${GEMM_BLOCK}"
                                      # do something
                                  echo  $T_oi " " $T_oj " " $T_ifm_tile " " $T_ofm_tile
                                  ./conv2d ${iters} ${ifw} ${ifh} ${nIfm} ${nOfm} ${kw} ${kh} ${pad_w} ${pad_h} ${stride} ${images} ${version} ${check_correctness} &> run_output
                                  GFLOPS=`cat run_output | grep Real_GFLOPS | cut -d"=" -f 2`
                                  ERROR=`cat run_output | grep "inf-norm of comp. abs. error" | cut -d: -f 2`
                                  RELERROR=`cat run_output | grep "inf-norm of comp. rel. error" | cut -d: -f 2`
                                  echo "GFLOPS="${GFLOPS}
                                  echo  "${Step_M_i}_${Step_N_j}_${Step_K_k},${GFLOPS},${ERROR},${RELERROR}" >> ${PERF_DIR}/${images}_${config_num}_${OUTPUT}
                                  # exit

                            fi
                        done
                        fi
                    done
                    fi
                done
              fi
          fi
          done
  done
