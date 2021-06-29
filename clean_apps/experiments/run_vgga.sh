
FILE=$1
ITERS=1000 
#vgga
 sh ./$FILE ${ITERS} 224 224       3   64  3  3 1 1 1  vgga_01 #
 sh ./$FILE ${ITERS} 112 112      64  128  3  3 1 1 1  vgga_02
 sh ./$FILE ${ITERS}  56  56     128  256  3  3 1 1 1  vgga_03
 sh ./$FILE ${ITERS}  56  56     256  256  3  3 1 1 1  vgga_04
 sh ./$FILE ${ITERS}  28  28     256  512  3  3 1 1 1  vgga_05
 sh ./$FILE ${ITERS}  28  28     512  512  3  3 1 1 1  vgga_06
 sh ./$FILE ${ITERS}  14  14     512  512  3  3 1 1 1  vgga_07
