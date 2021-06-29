
PERF_DIR=$1
SUMMARY_DIR=summary
SUMMARY_FILE=summary.csv

mkdir ${SUMMARY_DIR}
rm ${SUMMARY_DIR}/1_${SUMMARY_FILE}
rm ${SUMMARY_DIR}/28_${SUMMARY_FILE}

END=_poly_perf.csv_top1_perf.csv
for images in 1 28
 do
   for file in ${PERF_DIR}/*_${images}${END}
   do
     SUBSTRING=$(basename -- "$file")
     STRLENGTH=$(echo -n _${images}${END} | wc -m)
     config_num=${SUBSTRING:0:-$STRLENGTH}

     echo file: $file
     OUTPUT=$(sed -n '2p' ${file}) # >> ${SUMMARY_DIR}/${images}_${SUMMARY_FILE}
     echo $config_num,$OUTPUT >> ${SUMMARY_DIR}/${images}_${SUMMARY_FILE}
   done
 done


