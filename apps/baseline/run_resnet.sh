 
icc -O3 -fopenmp conv2d.c -o conv2d
#icc -O3 -xCORE-AVX512 -fopenmp conv2d.c -o conv2d
#icc -O3 -xCORE-AVX512 -qopt-matmul -fopenmp conv2d.c -o conv2d

FILE=run_conv.sh
ITERS=10
#Resnet-50
sh ./$FILE $ITERS  56  56  64  256 1 1 0 0 1 resnet_01
sh ./$FILE $ITERS  56  56  64  64 1 1 0 0 1 resnet_02
sh ./$FILE $ITERS  56  56  64  64 3 3 1 1 1 resnet_03
sh ./$FILE $ITERS  56  56  256  64 1 1 0 0 1 resnet_04
sh ./$FILE $ITERS  56  56  256  512 1 1 0 0 2 resnet_05
sh ./$FILE $ITERS  56  56  256   128 1 1 0 0 2 resnet_06
sh ./$FILE $ITERS  28  28  128   128 3 3 1 1 1 resnet_07
sh ./$FILE $ITERS  28  28  128   512 1 1 0 0 1 resnet_08
sh ./$FILE $ITERS  28  28  512   128 1 1 0 0 1 resnet_09
sh ./$FILE $ITERS  28  28  512  1024 1 1 0 0 2 resnet_10
sh ./$FILE $ITERS  28  28  512   256 1 1 0 0 2 resnet_11
sh ./$FILE $ITERS  14  14  256   256 3 3 1 1 1 resnet_12
sh ./$FILE $ITERS  14  14  256  1024 1 1 0 0 1 resnet_13
sh ./$FILE $ITERS  14  14  1024   256 1 1 0 0 1 resnet_14
sh ./$FILE $ITERS  14   14   1024  2048 1 1 0 0 2 resnet_15
sh ./$FILE $ITERS  14   14   1024   512 1 1 0 0 2 resnet_16
sh ./$FILE $ITERS  7   7   512   512 3 3 1 1 1 resnet_17
sh ./$FILE $ITERS  7   7   512  2048 1 1 0 0 1 resnet_18
sh ./$FILE $ITERS  7   7   2048   512 1 1 0 0 1 resnet_19
