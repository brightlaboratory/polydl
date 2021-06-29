FILE=$1
ITERS=1000
#Overfeat
 sh ./$FILE ${ITERS} 231 231       3   96 11 11 0 0 4  overfeat_01 #
 sh ./$FILE ${ITERS}  28  28      96  256  5  5 0 0 1  overfeat_02 
 sh ./$FILE ${ITERS}  12  12     256  512  3  3 1 1 1  overfeat_03
 sh ./$FILE ${ITERS}  12  12     512 1024  3  3 1 1 1  overfeat_04
 sh ./$FILE ${ITERS}  12  12    1024 1024  3  3 1 1 1  overfeat_05

