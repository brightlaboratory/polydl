FILE=$1
ITERS=1000
#yolov2
 sh ./$FILE ${ITERS} 610 610   3 32 3 3 0 0 1  yolov2_01 #
 sh ./$FILE ${ITERS} 306 306   32 64 3 3 0 0 1  yolov2_02 
 sh ./$FILE ${ITERS} 154 154   64 128 3 3 0 0 1  yolov2_03
 sh ./$FILE ${ITERS} 152 152   128 64 1 1 0 0 1  yolov2_04
 sh ./$FILE ${ITERS} 78 78   128 256 3 3 0 0 1  yolov2_05
 sh ./$FILE ${ITERS} 76 76   256 128 1 1 0 0 1  yolov2_06
 sh ./$FILE ${ITERS} 40 40   256 512 3 3 0 0 1  yolov2_07
 sh ./$FILE ${ITERS} 38 38   512 256 1 1 0 0 1  yolov2_08
 sh ./$FILE ${ITERS} 21 21   512 1024 3 3 0 0 1  yolov2_09
 sh ./$FILE ${ITERS} 19 19   1024 512 1 1 0 0 1  yolov2_10
 sh ./$FILE ${ITERS} 21 21   1024 1024 3 3 0 0 1  yolov2_11
 sh ./$FILE ${ITERS} 38 38   512 64 1 1 0 0 1  yolov2_12
 sh ./$FILE ${ITERS} 21 21   1280 1024 3 3 0 0 1  yolov2_13
 sh ./$FILE ${ITERS} 19 19   1024 425 1 1 0 0 1  yolov2_14 #
