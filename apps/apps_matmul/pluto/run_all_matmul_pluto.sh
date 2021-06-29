#!/bin/bash
ITERS=1
M=$1
N=$2
K=$3
NUM_THREADS=$4
echo M: $M N: $N K: $K NUM_THREADS: ${NUM_THREADS}
MIN=16
for (( M1_Tile=4; M1_Tile<=4; M1_Tile=M1_Tile*2 ))
do 
   if [ `expr $M % ${M1_Tile}` -eq 0 ]
   then

        for (( N1_Tile=${MIN}; N1_Tile<=$N; N1_Tile=N1_Tile*4 ))
        do
          if [ `expr $N % ${N1_Tile}` -eq 0 ]
          then

        	for (( K1_Tile=${MIN}; K1_Tile<=$K; K1_Tile=K1_Tile*4 ))
        	do
          	 if [ `expr $K % ${K1_Tile}` -eq 0 ]
         	 then

                        echo $M $N $K ${M1_Tile} ${N1_Tile} ${K1_Tile}
			sh ./run_matmul_pluto.sh ${ITERS} $M $N $K ${M1_Tile} ${N1_Tile} ${K1_Tile} ${NUM_THREADS}
          	 fi
        	done
          fi
        done
   fi
done
