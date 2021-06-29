export KMP_AFFINITY=granularity=fine,compact,1,0
OUT=perf.csv
rm ${OUT}

#Resne20
config1='100  56  56  64  256 1 1 0 0 1'
config2='100  56  56  64  64 1 1 0 0 1'
config3='100  56  56  64  64 3 3 1 1 1'
config4='100  56  56  256  64 1 1 0 0 1'
config5='100  28  28  256   512 1 1 0 0 1'
config6='100  28  28  256   128 1 1 0 0 1'
config7='100  28  28  128   128 3 3 1 1 1'
config8='100  28  28  128   512 1 1 0 0 1'
config9='100  28  28  512   128 1 1 0 0 1'
config10='100  14  14  512  1024 1 1 0 0 1'
config11='100  14  14  512   256 1 1 0 0 1'
config12='100  14  14  256   256 3 3 1 1 1'
config13='100  14  14  256  1024 1 1 0 0 1'
config14='100  14  14  1024   256 1 1 0 0 1'
config15='100  7   7   1024  2048 1 1 0 0 1'
config16='100  7   7   1024   512 1 1 0 0 1'
config17='100  7   7   512   512 3 3 1 1 1'
config18='100  7   7   512  2048 1 1 0 0 1'
config19='100  7   7   2048   512 1 1 0 0 1'

#Mobilenet
#config20='100  224 224 3 32 3 3 1 1 2'
config21='100  112 112 32 64 1 1 0 0 1'
config22='100  56 56  64 128 1 1 0 0 1'
config23='100  56 56  128 128 1 1 0 0 1'
config24='100  28 28  128 256 1 1 0 0 1'
config25='100  28 28  256 256 1 1 0 0 1'
config26='100  14 14  256 512 1 1 0 0 1'
config27='100  14 14  512 512 1 1 0 0 1'
config28='100  7 7 512  1024 1 1 0 0 1'
config29='100  7 7 1024 1024 1 1 0 0 1'
config30='100  1 1 1024  5 1 1 0 0 1'

#Alexnet
#config31='100  227 227 3  64 11 11 0 0 4'
config32='100  27  27  64 192 5 5 2 2 1'
config33='100  13  13  192 384 3 3 1 1 1'
config34='100  13  13  384 256 3 3 1 1 1'
config35='100  13  13  256 256 3 3 1 1 1'

for config in "$config1" "$config2" "$config3" "$config4" "$config5" "$config6" "$config7" "$config8" "$config9" "$config10" "$config11" "$config12" "$config13" "$config14" "$config15" "$config16" "$config17" "$config18" "$config19"  "$config21" "$config22" "$config23" "$config24" "$config25" "$config26" "$config27" "$config28" "$config29" "$config30" "$config32" "$config33" "$config34" "$config35"

do
	echo -n $config, >> ${OUT}
	for images in 1 28
	do
		echo -n ${images}, >> ${OUT}
		export OMP_NUM_THREADS=${images}
		for version in 0 1 2 3 4 5
		do
			echo "Config: " $config ${images} ${version}
			GFLOPS=`./conv2d $config ${images} ${version} |  grep GFLOPS |  cut -d= -f2`
			#GFLOPS=`./conv2d $config ${images} ${version}`
			#echo $GFLOPS
			echo -n "${version},${GFLOPS}," >> ${OUT}
		done
	done
	echo >> ${OUT}
done

