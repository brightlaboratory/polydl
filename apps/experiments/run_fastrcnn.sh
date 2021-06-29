
FILE=$1
ITERS=1000

sh ./$FILE ${ITERS} 606 756   3 64 7 7 0 0 2 fastrcnn_01 #
sh ./$FILE ${ITERS} 150 188   64 64 1 1 0 0 1  fastrcnn_02
sh ./$FILE ${ITERS} 150 188   64 64 3 3 1 1 1  fastrcnn_03
sh ./$FILE ${ITERS} 150 188   64 256 1 1 0 0 1  fastrcnn_04
sh ./$FILE ${ITERS} 150 188   256 64 1 1 0 0 1  fastrcnn_05
sh ./$FILE ${ITERS} 152 190   64 64 3 3 0 0 2  fastrcnn_06
sh ./$FILE ${ITERS} 75 94   64 256 1 1 0 0 1  fastrcnn_07
sh ./$FILE ${ITERS} 75 94   256 128 1 1 0 0 1  fastrcnn_08
sh ./$FILE ${ITERS} 75 94   256 512 1 1 0 0 1  fastrcnn_09
sh ./$FILE ${ITERS} 75 94   128 128 3 3 1 1 1  fastrcnn_10
sh ./$FILE ${ITERS} 75 94   128 512 1 1 0 0 1  fastrcnn_11
sh ./$FILE ${ITERS} 75 94   512 128 1 1 0 0 1  fastrcnn_12
sh ./$FILE ${ITERS} 77 96   128 128 3 3 0 0 2  fastrcnn_13
sh ./$FILE ${ITERS} 38 47   128 512 1 1 0 0 1  fastrcnn_14
sh ./$FILE ${ITERS} 38 47   512 256 1 1 0 0 1  fastrcnn_15
sh ./$FILE ${ITERS} 38 47   256 256 3 3 1 1 1  fastrcnn_16
sh ./$FILE ${ITERS} 38 47   256 1024 1 1 0 0 1  fastrcnn_17
sh ./$FILE ${ITERS} 38 47   512 1024 1 1 0 0 1  fastrcnn_18
sh ./$FILE ${ITERS} 38 47   1024 256 1 1 0 0 1  fastrcnn_19
sh ./$FILE ${ITERS} 38 47   1024 512 3 3 1 1 1  fastrcnn_20
sh ./$FILE ${ITERS} 38 47   512 48 1 1 0 0 1  fastrcnn_21 
sh ./$FILE ${ITERS} 38 47   512 24 1 1 0 0 1  fastrcnn_22 #
sh ./$FILE ${ITERS} 7 7   1024 512 1 1 0 0 1  fastrcnn_23
sh ./$FILE ${ITERS} 7 7   512 512 3 3 1 1 1  fastrcnn_24
sh ./$FILE ${ITERS} 7 7   512 2048 1 1 0 0 1  fastrcnn_25
sh ./$FILE ${ITERS} 7 7   1024 2048 1 1 0 0 1  fastrcnn_26
sh ./$FILE ${ITERS} 7 7   2048 512 1 1 0 0 1  fastrcnn_27
