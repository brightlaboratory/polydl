FILE=$1
ITERS=1000
#mobilenet
 sh ./$FILE ${ITERS} 224 224   3 32 3 3 1 1 2  mobilenet_01 #
 sh ./$FILE ${ITERS} 112 112   32 64 1 1 0 0 1  mobilenet_02 
 sh ./$FILE ${ITERS} 56 56   64 128 1 1 0 0 1  mobilenet_03
 sh ./$FILE ${ITERS} 56 56   128 128 1 1 0 0 1  mobilenet_04
 sh ./$FILE ${ITERS} 28 28   128 256 1 1 0 0 1  mobilenet_05
 sh ./$FILE ${ITERS} 28 28   256 256 1 1 0 0 1  mobilenet_06
 sh ./$FILE ${ITERS} 14 14   256 512 1 1 0 0 1  mobilenet_07
 sh ./$FILE ${ITERS} 14 14   512 512 1 1 0 0 1  mobilenet_08
 sh ./$FILE ${ITERS} 7 7   512 1024 1 1 0 0 1  mobilenet_09
 sh ./$FILE ${ITERS} 7 7   1024 1024 1 1 0 0 1  mobilenet_10
 sh ./$FILE ${ITERS} 1 1   1024 5 1 1 0 0 1  mobilenet_11 #
