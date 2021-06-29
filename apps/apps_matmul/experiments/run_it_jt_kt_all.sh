#!/bin/bash

FILE=matmul3.c
ITERS=5
M=$1
N=$2
K=$3

echo M: $M N: $N K: $K

for (( M2_Tile=64; M2_Tile<=$M; M2_Tile=M2_Tile*2 ))
do  
   if [ `expr $M % ${M2_Tile}` -eq 0 ]
   then

        for (( N2_Tile=64; N2_Tile<=$N; N2_Tile=N2_Tile*2 ))
        do
          if [ `expr $N % ${N2_Tile}` -eq 0 ]
          then

        	for (( K2_Tile=64; K2_Tile<=$K; K2_Tile=K2_Tile*2 ))
        	do
          	 if [ `expr $K % ${K2_Tile}` -eq 0 ]
         	 then
			sh ./run_matmul.sh $ITERS $M $N $K ${M2_Tile} ${N2_Tile} ${K2_Tile} 64 64 64 $FILE 
          	 fi
        	done
          fi
        done
   fi
done
