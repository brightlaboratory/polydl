
SUMMARY_DIR=summary
SUMMARY_FILE=summary.csv

PERF_DIR=perf_data
OUT=poly_perf.csv
SUFFIX=_top1_perf.csv

mkdir ${SUMMARY_DIR}
rm ${SUMMARY_DIR}/1_${SUMMARY_FILE}
rm ${SUMMARY_DIR}/28_${SUMMARY_FILE}
for config_num in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19
  do
    for images in 1 28
      do
        RANKS_OUT=${PERF_DIR}/${config_num}_${images}_${OUT}${SUFFIX}
	echo -n "${config_num},${images}," | sed -n '2p' ${RANKS_OUT} >> ${SUMMARY_DIR}/${images}_${SUMMARY_FILE}      
      done
  done

