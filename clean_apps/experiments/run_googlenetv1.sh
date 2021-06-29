
FILE=$1
ITERS=1000 
#Googlenetv1
 sh ./$FILE ${ITERS}   224  224        3    64  7  7  3  3  2  googlenetv1_01 #
 sh ./$FILE ${ITERS}    56   56       64    64  1  1  0  0  1  googlenetv1_02
 sh ./$FILE ${ITERS}    56   56       64   192  3  3  1  1  1  googlenetv1_03
 sh ./$FILE ${ITERS}    28   28      192    64  1  1  0  0  1  googlenetv1_04
 sh ./$FILE ${ITERS}    28   28      192    96  1  1  0  0  1  googlenetv1_05
 sh ./$FILE ${ITERS}    28   28       96   128  3  3  1  1  1  googlenetv1_06
 sh ./$FILE ${ITERS}    28   28      192    16  1  1  0  0  1  googlenetv1_07
 sh ./$FILE ${ITERS}    28   28       16    32  5  5  2  2  1  googlenetv1_08
 sh ./$FILE ${ITERS}    28   28      192    32  1  1  0  0  1  googlenetv1_09
 sh ./$FILE ${ITERS}    28   28      256   128  1  1  0  0  1  googlenetv1_10
 sh ./$FILE ${ITERS}    28   28      128   192  3  3  1  1  1  googlenetv1_11
 sh ./$FILE ${ITERS}    28   28      256    32  1  1  0  0  1  googlenetv1_12
 sh ./$FILE ${ITERS}    28   28       32    96  5  5  2  2  1  googlenetv1_13
 sh ./$FILE ${ITERS}    28   28      256    64  1  1  0  0  1  googlenetv1_14
 sh ./$FILE ${ITERS}    14   14      480   192  1  1  0  0  1  googlenetv1_15
 sh ./$FILE ${ITERS}    14   14      480    96  1  1  0  0  1  googlenetv1_16
 sh ./$FILE ${ITERS}    14   14       96   208  3  3  1  1  1  googlenetv1_17
 sh ./$FILE ${ITERS}    14   14      480    16  1  1  0  0  1  googlenetv1_18
 sh ./$FILE ${ITERS}    14   14       16    48  5  5  2  2  1  googlenetv1_19
 sh ./$FILE ${ITERS}    14   14      480    64  1  1  0  0  1  googlenetv1_20
 sh ./$FILE ${ITERS}    14   14      512   160  1  1  0  0  1  googlenetv1_21
 sh ./$FILE ${ITERS}    14   14      512   112  1  1  0  0  1  googlenetv1_22
 sh ./$FILE ${ITERS}    14   14      112   224  3  3  1  1  1  googlenetv1_23
 sh ./$FILE ${ITERS}    14   14      512    32  1  1  0  0  1  googlenetv1_24
 sh ./$FILE ${ITERS}    14   14       32    64  5  5  2  2  1  googlenetv1_25
 sh ./$FILE ${ITERS}    14   14      512    64  1  1  0  0  1  googlenetv1_26
 sh ./$FILE ${ITERS}    14   14      512   128  1  1  0  0  1  googlenetv1_27
 sh ./$FILE ${ITERS}    14   14      128   256  3  3  1  1  1  googlenetv1_28
 sh ./$FILE ${ITERS}    14   14      512   144  1  1  0  0  1  googlenetv1_29
 sh ./$FILE ${ITERS}    14   14      144   288  3  3  1  1  1  googlenetv1_30
 sh ./$FILE ${ITERS}    14   14      512    32  1  1  0  0  1  googlenetv1_31
 sh ./$FILE ${ITERS}    14   14       32    64  5  5  2  2  1  googlenetv1_32
 sh ./$FILE ${ITERS}    14   14      528   256  1  1  0  0  1  googlenetv1_33
 sh ./$FILE ${ITERS}    14   14      528   160  1  1  0  0  1  googlenetv1_34
 sh ./$FILE ${ITERS}    14   14      160   320  3  3  1  1  1  googlenetv1_35
 sh ./$FILE ${ITERS}    14   14      528    32  1  1  0  0  1  googlenetv1_36
 sh ./$FILE ${ITERS}    14   14       32   128  5  5  2  2  1  googlenetv1_37
 sh ./$FILE ${ITERS}    14   14      528   128  1  1  0  0  1  googlenetv1_38
 sh ./$FILE ${ITERS}     7    7      832   256  1  1  0  0  1  googlenetv1_39
 sh ./$FILE ${ITERS}     7    7      832   160  1  1  0  0  1  googlenetv1_40
 sh ./$FILE ${ITERS}     7    7      160   320  3  3  1  1  1  googlenetv1_41
 sh ./$FILE ${ITERS}     7    7      832    32  1  1  0  0  1  googlenetv1_42
 sh ./$FILE ${ITERS}     7    7       32   128  5  5  2  2  1  googlenetv1_43
 sh ./$FILE ${ITERS}     7    7      832   128  1  1  0  0  1  googlenetv1_44
 sh ./$FILE ${ITERS}     7    7      832   384  1  1  0  0  1  googlenetv1_45
 sh ./$FILE ${ITERS}     7    7      832   192  1  1  0  0  1  googlenetv1_46
 sh ./$FILE ${ITERS}     7    7      192   384  3  3  1  1  1  googlenetv1_47
 sh ./$FILE ${ITERS}     7    7      832    48  1  1  0  0  1  googlenetv1_48
 sh ./$FILE ${ITERS}     7    7       48   128  5  5  2  2  1  googlenetv1_49

