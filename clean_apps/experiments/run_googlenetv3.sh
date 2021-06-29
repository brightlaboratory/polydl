FILE=$1
ITERS=1000 
#Googlenetv3
 sh ./$FILE ${ITERS}  299 299       3   32 3 3 0 0 2  googlenetv3_01 #
 sh ./$FILE ${ITERS}  149 149      32   32 3 3 0 0 1  googlenetv3_02
 sh ./$FILE ${ITERS}  147 147      32   64 3 3 1 1 1  googlenetv3_03
 sh ./$FILE ${ITERS}  73  73       64   80 3 3 0 0 1  googlenetv3_04
 sh ./$FILE ${ITERS}  73  73       80  192 3 3 0 0 2  googlenetv3_05
 sh ./$FILE ${ITERS}  71  71       80  192 3 3 0 0 1  googlenetv3_06
 sh ./$FILE ${ITERS}  35  35      192   64 1 1 0 0 1  googlenetv3_07
 sh ./$FILE ${ITERS}  35  35       64   96 3 3 1 1 1  googlenetv3_08
 sh ./$FILE ${ITERS}  35  35       96   96 3 3 1 1 1  googlenetv3_09
 sh ./$FILE ${ITERS}  35  35      192   48 1 1 0 0 1  googlenetv3_10
 sh ./$FILE ${ITERS}  35  35       48   64 5 5 2 2 1  googlenetv3_11
 sh ./$FILE ${ITERS}  35  35      192   32 1 1 0 0 1  googlenetv3_12
 sh ./$FILE ${ITERS}  35  35      256   64 1 1 0 0 1  googlenetv3_13
 sh ./$FILE ${ITERS}  35  35      256   48 1 1 0 0 1  googlenetv3_14
 sh ./$FILE ${ITERS}  35  35      288   64 1 1 0 0 1  googlenetv3_15
 sh ./$FILE ${ITERS}  35  35      288   48 1 1 0 0 1  googlenetv3_16
 sh ./$FILE ${ITERS}  35  35       96   96 3 3 0 0 2  googlenetv3_17
 sh ./$FILE ${ITERS}  35  35      288  384 3 3 0 0 2  googlenetv3_18
 sh ./$FILE ${ITERS}  17  17      768  128 1 1 0 0 1  googlenetv3_19
 sh ./$FILE ${ITERS}  17  17      128  128 1 7 0 3 1  googlenetv3_20
 sh ./$FILE ${ITERS}  17  17      128  128 7 1 3 0 1  googlenetv3_21
 sh ./$FILE ${ITERS}  17  17      128  192 7 1 3 0 1  googlenetv3_22
 sh ./$FILE ${ITERS}  17  17      128  192 1 7 0 3 1  googlenetv3_23
 sh ./$FILE ${ITERS}  17  17      768  192 1 1 0 0 1  googlenetv3_24
 sh ./$FILE ${ITERS}  17  17      768  160 1 1 0 0 1  googlenetv3_25
 sh ./$FILE ${ITERS}  17  17      160  160 1 7 0 3 1  googlenetv3_26
 sh ./$FILE ${ITERS}  17  17      160  160 7 1 3 0 1  googlenetv3_27
 sh ./$FILE ${ITERS}  17  17      160  192 7 1 3 0 1  googlenetv3_28
 sh ./$FILE ${ITERS}  17  17      160  192 1 7 0 3 1  googlenetv3_29
 sh ./$FILE ${ITERS}  17  17      192  192 1 7 0 3 1  googlenetv3_30
 sh ./$FILE ${ITERS}  17  17      192  192 7 1 3 0 1  googlenetv3_31
 sh ./$FILE ${ITERS}  17  17      192  192 3 3 0 0 2  googlenetv3_32
 sh ./$FILE ${ITERS}  17  17      192  320 3 3 0 0 2  googlenetv3_33
 sh ./$FILE ${ITERS}  8   8      1280  320 1 1 0 0 1  googlenetv3_34
 sh ./$FILE ${ITERS}  8   8      1280  192 1 1 0 0 1  googlenetv3_35
 sh ./$FILE ${ITERS}  8   8      1280  448 1 1 0 0 1  googlenetv3_36 
 sh ./$FILE ${ITERS}  8   8       448  384 3 3 1 1 1  googlenetv3_37 #
 sh ./$FILE ${ITERS}  8   8       384  384 1 3 0 1 1  googlenetv3_38 #
 sh ./$FILE ${ITERS}  8   8       384  384 3 1 1 0 1  googlenetv3_39
 sh ./$FILE ${ITERS}  8   8      1280  384 1 1 0 0 1  googlenetv3_40 #
 sh ./$FILE ${ITERS}  8   8      2048  320 1 1 0 0 1  googlenetv3_41
 sh ./$FILE ${ITERS}  8   8      2048  192 1 1 0 0 1  googlenetv3_42
 sh ./$FILE ${ITERS}  8   8      2048  448 1 1 0 0 1  googlenetv3_43
 sh ./$FILE ${ITERS}  8   8      2048  384 1 1 0 0 1  googlenetv3_44 #
 sh ./$FILE ${ITERS}  8   8       384  384 1 3 0 1 1  googlenetv3_45 #

