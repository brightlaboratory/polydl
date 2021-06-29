#Example usage: sh cleanse.sh resnet_05_28_poly_perf.csv 1 48 
FILE=$1
META=meta_$FILE
BEGIN=$2
END=$3
echo FILE: $FILE , BEGIN: $BEGIN, END: $END
sed -n "${BEGIN},${END}p" < $FILE  >> cleansed_$FILE
sed -n "${BEGIN},${END}p" < $META  >> cleansed_$META

