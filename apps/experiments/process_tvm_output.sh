
for RUN_OUTPUT in tvm_output/*; do
	GFLOPS=`cat ${RUN_OUTPUT} | tr -s ' ' | grep "GFLOPS" |rev| cut -d "|" -f3|rev | cut -d " " -f7 | cut -d "/" -f2`
	echo $RUN_OUTPUT,$GFLOPS
done


