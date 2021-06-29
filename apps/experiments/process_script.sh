HAYSTACK_FILE=$1
OUTPUT_FILE=gflops_${HAYSTACK_FILE}
arch=`echo $HAYSTACK_FILE | cut -d "_" -f1 `
layer=`echo $HAYSTACK_FILE | cut -d "_" -f2 `
layerint=`echo $((layer))`
GFLOPS_FILE="../raw_data/cleansed_${arch}/${arch}_${layerint}_28_poly_perf.csv"
TEMP_FILE=temp
VERSION=`cat ${HAYSTACK_FILE} | cut -d "," -f1`
GFLOPS=`cat ${GFLOPS_FILE} | grep -w $VERSION | cut -d "," -f2`
echo ,$GFLOPS > ${TEMP_FILE}
head -c -1 -q ${HAYSTACK_FILE} ${TEMP_FILE} > ${OUTPUT_FILE}
