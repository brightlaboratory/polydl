#!/bin/bash
FILE=matmul3.c
ITERS=100
M=$1
N=$2
K=$3
INPUT_NUM_THREADS=$4
let NUM_THREADS="( ${INPUT_NUM_THREADS} ) "
echo M: $M N: $N K: $K NUM_THREADS: ${NUM_THREADS}
MIN=16
for (( M2_Tile=${MIN}; M2_Tile<=$M; M2_Tile=M2_Tile*2 ))
do 
   if [ `expr $M % ${M2_Tile}` -eq 0 ]
   then

        for (( N2_Tile=${MIN}; N2_Tile<=$N; N2_Tile=N2_Tile*2 ))
        do
          if [ `expr $N % ${N2_Tile}` -eq 0 ]
          then

        	for (( K2_Tile=${MIN}; K2_Tile<=$K; K2_Tile=K2_Tile*2 ))
        	do
          	 if [ `expr $K % ${K2_Tile}` -eq 0 ]
         	 then

				for (( M1_Tile=16; M1_Tile<=64; M1_Tile=M1_Tile*2 ))
				do  
				 if [ `expr ${M2_Tile} % ${M1_Tile}` -eq 0 ]
				then

				for (( N1_Tile=16; N1_Tile<=64; N1_Tile=N1_Tile*2 ))
				do  
				 if [ `expr ${N2_Tile} % ${N1_Tile}` -eq 0 ]
				then

				for (( K1_Tile=16; K1_Tile<=64; K1_Tile=K1_Tile*2 ))
				do 
				 if [ `expr ${K2_Tile} % ${K1_Tile}` -eq 0 ]
				then

                                 if(( $M1_Tile == $N1_Tile && $N1_Tile == $K1_Tile ))
				 then
			       # echo $M $N $K ${M2_Tile} ${N2_Tile} ${K2_Tile} ${M1_Tile} ${N1_Tile} ${K1_Tile}
					let n="( $M / ${M2_Tile} ) % $NUM_THREADS "
					if [ $n -eq 0 ]
					then
						echo $M $N $K ${M2_Tile} ${N2_Tile} ${K2_Tile} ${M1_Tile} ${N1_Tile} ${K1_Tile} it2
						 sh ./run_matmul.sh $ITERS $M $N $K ${M2_Tile} ${N2_Tile} ${K2_Tile} ${M1_Tile} ${N1_Tile} ${K1_Tile} $FILE it2 ${NUM_THREADS}
					fi

			fi
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

OUT=poly_perf.csv
PERF_DIR=perf_data
CONFIG_OUT=${PERF_DIR}/${file}_${M}_${N}_${K}_${OUT}
../../../scripts/polyrank ${CONFIG_OUT} --noheader --perfseparaterow --usepessidata ${CONFIG_OUT}
