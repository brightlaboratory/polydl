export KMP_AFFINITY=granularity=fine,compact,1,0
OUT=perf.csv
rm ${OUT}

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

for config in "$config1" "$config2" "$config3" "$config4" "$config5" "$config6" "$config7" "$config8" "$config9" "$config10" "$config11" "$config12" "$config13" "$config14" "$config15" "$config16" "$config17" "$config18" "$config19"
do
echo -n $config, >> ${OUT}
for images in 1 28
do
echo -n ${images}, >> ${OUT}
export OMP_NUM_THREADS=${images}
for version in 0 1 2 3 4 5 7
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

