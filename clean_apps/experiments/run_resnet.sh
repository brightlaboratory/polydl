#!/bin/bash
FILE=$1
VERSION_FILE=$2
ITERS=1000
#Resnet-50
#bash ./$FILE $ITERS  224 224 3   64 7 7 3 3 2 resnet_00 $VERSION_FILE #
 bash ./$FILE $ITERS  56  56  64  256 1 1 0 0 1 resnet_01 $VERSION_FILE 
# bash ./$FILE $ITERS  56  56  64  64 1 1 0 0 1 resnet_02 $VERSION_FILE 
# bash ./$FILE $ITERS  56  56  64  64 3 3 1 1 1 resnet_03 $VERSION_FILE 
# bash ./$FILE $ITERS  56  56  256  64 1 1 0 0 1 resnet_04 $VERSION_FILE 
# bash ./$FILE $ITERS  56  56  256  512 1 1 0 0 2 resnet_05 $VERSION_FILE 
# bash ./$FILE $ITERS  56  56  256   128 1 1 0 0 2 resnet_06 $VERSION_FILE 
# bash ./$FILE $ITERS  28  28  128   128 3 3 1 1 1 resnet_07 $VERSION_FILE 
# bash ./$FILE $ITERS  28  28  128   512 1 1 0 0 1 resnet_08 $VERSION_FILE 
# bash ./$FILE $ITERS  28  28  512   128 1 1 0 0 1 resnet_09 $VERSION_FILE 
# bash ./$FILE $ITERS  28  28  512  1024 1 1 0 0 2 resnet_10 $VERSION_FILE 
# bash ./$FILE $ITERS  28  28  512   256 1 1 0 0 2 resnet_11 $VERSION_FILE 
# bash ./$FILE $ITERS  14  14  256   256 3 3 1 1 1 resnet_12 $VERSION_FILE 
# bash ./$FILE $ITERS  14  14  256  1024 1 1 0 0 1 resnet_13 $VERSION_FILE 
# bash ./$FILE $ITERS  14  14  1024   256 1 1 0 0 1 resnet_14 $VERSION_FILE 
# bash ./$FILE $ITERS  14   14   1024  2048 1 1 0 0 2 resnet_15 $VERSION_FILE 
# bash ./$FILE $ITERS  14   14   1024   512 1 1 0 0 2 resnet_16 $VERSION_FILE 
# bash ./$FILE $ITERS  7   7   512   512 3 3 1 1 1 resnet_17 $VERSION_FILE 
# bash ./$FILE $ITERS  7   7   512  2048 1 1 0 0 1 resnet_18 $VERSION_FILE 
# bash ./$FILE $ITERS  7   7   2048   512 1 1 0 0 1 resnet_19 $VERSION_FILE 
