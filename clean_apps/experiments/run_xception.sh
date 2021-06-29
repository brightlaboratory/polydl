
FILE=$1
ITERS=1000
#xception
 sh ./$FILE ${ITERS} 299 299   3 32 3 3 0 0 2  xception_01 #
 sh ./$FILE ${ITERS} 149 149   32 64 3 3 0 0 1  xception_02 
 sh ./$FILE ${ITERS} 147 147   64 128 1 1 0 0 1  xception_03
 sh ./$FILE ${ITERS} 147 147   128 128 1 1 0 0 1  xception_04
 sh ./$FILE ${ITERS} 74 74   128 256 1 1 0 0 1  xception_05
 sh ./$FILE ${ITERS} 74 74   256 256 1 1 0 0 1  xception_06
sh ./$FILE ${ITERS} 37 37   256 728 1 1 0 0 1  xception_07 #
 sh ./$FILE ${ITERS} 37 37   728 728 1 1 0 0 1  xception_08 #
 sh ./$FILE ${ITERS} 19 19   728 728 1 1 0 0 1  xception_09 #
 sh ./$FILE ${ITERS} 19 19   728 1024 1 1 0 0 1  xception_10 #
 sh ./$FILE ${ITERS} 10 10   1024 1536 1 1 0 0 1  xception_11
 sh ./$FILE ${ITERS} 10 10   1536 2048 1 1 0 0 1  xception_12
