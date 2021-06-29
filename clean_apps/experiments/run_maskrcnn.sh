
 FILE=$1
 ITERS=1000
 sh ./$FILE ${ITERS} 1030 1030   3 64 7 7 0 0 2  maskrcnn_01 #
 sh ./$FILE ${ITERS} 256 256   64 256 1 1 0 0 1  maskrcnn_02
 sh ./$FILE ${ITERS} 256 256   64 64 1 1 0 0 1  maskrcnn_03
 sh ./$FILE ${ITERS} 256 256   64 64 3 3 1 1 1  maskrcnn_04
 sh ./$FILE ${ITERS} 256 256   256 64 1 1 0 0 1  maskrcnn_05
 sh ./$FILE ${ITERS} 256 256   256 128 1 1 0 0 2  maskrcnn_06
 sh ./$FILE ${ITERS} 256 256   256 256 1 1 0 0 1  maskrcnn_07
 sh ./$FILE ${ITERS} 256 256   256 512 1 1 0 0 2  maskrcnn_08
 sh ./$FILE ${ITERS} 128 128   128 128 3 3 1 1 1  maskrcnn_09
 sh ./$FILE ${ITERS} 128 128   128 512 1 1 0 0 1  maskrcnn_10
 sh ./$FILE ${ITERS} 128 128   512 128 1 1 0 0 1  maskrcnn_11
 sh ./$FILE ${ITERS} 128 128   512 256 1 1 0 0 2  maskrcnn_12
 sh ./$FILE ${ITERS} 64 64   256 256 3 3 1 1 1  maskrcnn_13
 sh ./$FILE ${ITERS} 64 64   256 1024 1 1 0 0 1  maskrcnn_14
 sh ./$FILE ${ITERS} 128 128   512 256 1 1 0 0 1  maskrcnn_15
 sh ./$FILE ${ITERS} 128 128   512 1024 1 1 0 0 2  maskrcnn_16
 sh ./$FILE ${ITERS} 64 64   1024 256 1 1 0 0 1  maskrcnn_17
 sh ./$FILE ${ITERS} 64 64   1024 2048 1 1 0 0 2  maskrcnn_18
 sh ./$FILE ${ITERS} 64 64   1024 512 1 1 0 0 2  maskrcnn_19
 sh ./$FILE ${ITERS} 32 32   512 512 3 3 1 1 1  maskrcnn_20
 sh ./$FILE ${ITERS} 32 32   512 2048 1 1 0 0 1  maskrcnn_21
 sh ./$FILE ${ITERS} 32 32   2048 512 1 1 0 0 1  maskrcnn_22
 sh ./$FILE ${ITERS} 32 32   2048 256 1 1 0 0 1  maskrcnn_23
 sh ./$FILE ${ITERS} 32 32   256 256 3 3 1 1 1  maskrcnn_24
 sh ./$FILE ${ITERS} 32 32   256 512 3 3 1 1 1  maskrcnn_25
 sh ./$FILE ${ITERS} 32 32   512 6 1 1 0 0 1  maskrcnn_26 #
 sh ./$FILE ${ITERS} 32 32   512 12 1 1 0 0 1  maskrcnn_27 #
 sh ./$FILE ${ITERS} 16 16   256 512 3 3 1 1 1  maskrcnn_28
 sh ./$FILE ${ITERS} 16 16   512 6 1 1 0 0 1  maskrcnn_29 #
 sh ./$FILE ${ITERS} 16 16   512 12 1 1 0 0 1  maskrcnn_30 #
 sh ./$FILE ${ITERS} 64 64   256 512 3 3 1 1 1  maskrcnn_31
 sh ./$FILE ${ITERS} 64 64   512 6 1 1 0 0 1  maskrcnn_32 #
 sh ./$FILE ${ITERS} 64 64   512 12 1 1 0 0 1  maskrcnn_33 #
 sh ./$FILE ${ITERS} 128 128   256 256 3 3 1 1 1  maskrcnn_34
 sh ./$FILE ${ITERS} 128 128   256 512 3 3 1 1 1  maskrcnn_35
 sh ./$FILE ${ITERS} 128 128   512 6 1 1 0 0 1  maskrcnn_36 #
 sh ./$FILE ${ITERS} 128 128   512 12 1 1 0 0 1  maskrcnn_37 #
 sh ./$FILE ${ITERS} 256 256   256 256 3 3 1 1 1  maskrcnn_38
 sh ./$FILE ${ITERS} 256 256   256 512 3 3 1 1 1  maskrcnn_39
 sh ./$FILE ${ITERS} 256 256   512 6 1 1 0 0 1  maskrcnn_40 #
 sh ./$FILE ${ITERS} 256 256   512 12 1 1 0 0 1  maskrcnn_41 #
 sh ./$FILE ${ITERS} 7 7   256 1024 7 7 0 0 1  maskrcnn_42
 sh ./$FILE ${ITERS} 1 1   1024 1024 1 1 0 0 1  maskrcnn_43
 sh ./$FILE ${ITERS} 14 14   256 256 3 3 1 1 1  maskrcnn_44
 sh ./$FILE ${ITERS} 28 28   256 256 2 2 0 0 2  maskrcnn_45
 sh ./$FILE ${ITERS} 28 28   256 81 1 1 0 0 1  maskrcnn_46 #
