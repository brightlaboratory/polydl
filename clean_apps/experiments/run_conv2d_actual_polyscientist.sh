export KMP_AFFINITY=granularity=fine,compact,1,28
export LD_LIBRARY_PATH=/nfs_home/stavarag/work/software/barvinok/barvinok-0.41.2_install/lib:/nfs_home/stavarag/work/software/barvinok/isl_install/lib:$LD_LIBRARY_PATH

OUT=poly_perf.csv

#config0='1000  224  224  3  64 7 7 3 3 2'
config1='1  56  56  64  256 1 1 0 0 1'
config2='1  56  56  64  64 1 1 0 0 1'
config3='1  56  56  64  64 3 3 1 1 1'
config4='1  56  56  256  64 1 1 0 0 1'
config5='1  56  56  256  512 1 1 0 0 2'
config6='1  56  56  256   128 1 1 0 0 2'
config7='1  28  28  128   128 3 3 1 1 1'
config8='1  28  28  128   512 1 1 0 0 1'
config9='1  28  28  512   128 1 1 0 0 1'
config10='1  28  28  512  1024 1 1 0 0 2'
config11='1  28  28  512   256 1 1 0 0 2'
config12='1  14  14  256   256 3 3 1 1 1'
config13='1  14  14  256  1024 1 1 0 0 1'
config14='1  14  14  1024   256 1 1 0 0 1'
config15='1  14   14   1024  2048 1 1 0 0 2'
config16='1  14   14   1024   512 1 1 0 0 2'
config17='1  7   7   512   512 3 3 1 1 1'
config18='1  7   7   512  2048 1 1 0 0 1'
config19='1  7   7   2048   512 1 1 0 0 1'

GEMM_BLOCK=64
config_num=1  #FIXME
check_correctness=1
PERF_DIR=perf_data
CONFIG_DIR=configs
TEMP=temp
GEMM_VERSIONS='0 1 2 3 4 5'
NONGEMM_VERSIONS='20 21 22 23 24 25'    

mkdir ${PERF_DIR}
mkdir ${TEMP}
for config in "$config1" "$config2" "$config3" "$config4" "$config5" "$config6" "$config7" "$config8" "$config9" "$config10" "$config11" "$config12" "$config13" "$config14" "$config15" "$config16" "$config17" "$config18" "$config19" #FIXME 
do
       params=( ${config} )
       ifw=${params[1]}
       ifh=${params[2]}
       nIfm=${params[3]}
       nOfm=${params[4]}
       kw=${params[5]}
       kh=${params[6]}
       pad_w=${params[7]}
       pad_h=${params[8]}
       stride=${params[9]}
       let ofh="($ifh + 2 * $pad_h - $kh)/$stride + 1"
       let ofw="($ifw + 2 * $pad_w - $kw)/$stride + 1"

	for images in 1 28
	do
	        CONFIG_OUT=${PERF_DIR}/${config_num}_${images}_${OUT}
		META_CONFIG_OUT=${PERF_DIR}/meta_${config_num}_${images}_${OUT}
	        rm ${CONFIG_OUT}
		rm ${META_CONFIG_OUT}

		export OMP_NUM_THREADS=${images}
		for version in $NONGEMM_VERSIONS #FIXME
		do
			#We will first do an actual run
			if [ $version -eq 0 -o $version -eq 1 -o $version -eq 20 -o $version -eq 21 ]
			then
				for (( T_oi=${ofw}; T_oi<= ${ofw}; T_oi=T_oi*4 ))
				do
				if [ `expr $ofw % $T_oi` -eq 0 ] 
				then
				for (( T_oj=${ofh}; T_oj >= 1; T_oj=T_oj/2 ))
				do
				if [ `expr $ofh % $T_oj` -eq 0 ] 
				then
				for (( T_ifm_tile=${nIfm}; T_ifm_tile<= ${nIfm}; T_ifm_tile=T_ifm_tile*2 ))
                                do
				if [ `expr $nIfm % $T_ifm_tile` -eq 0 ]
				then
                                for (( T_ofm_tile=${nOfm}; T_ofm_tile<= ${nOfm}; T_ofm_tile=T_ofm_tile*2 ))
                                do
                                if [ `expr $nOfm % $T_ofm_tile` -eq 0 ]
                                then

					(cd .. && make clean && make MACROFLAGS="-DSTRIDE_H=$stride -DSTRIDE_W=$stride -DT_oi=$T_oi -DT_oj=$T_oj -DT_ifm_tile=$T_ifm_tile -DT_ofm_tile=$T_ofm_tile")
     					# do something
					echo  $T_oi " " $T_oj " " $T_ifm_tile " " $T_ofm_tile
					../conv2d $config ${images} ${version} ${check_correctness} &> run_output
					GFLOPS=`cat run_output |  grep Real_GFLOPS |  cut -d= -f2`
					NAIVE_GFLOPS=`cat run_output |  grep Naive_GFLOPS |  cut -d= -f2` 
					ERROR=`cat run_output | grep "inf-norm of comp. abs. error" | cut -d: -f 2`
					rm ${TEMP}/temp.c
					if [ $version -eq 0 -o $version -eq 20 ] 
					then
					cp ../padded_conv_fp_tiled_loop_order_0.c ${TEMP}/temp.c
					fi

					if [ $version -eq 1 -o $version -eq 21 ] 
					then
					cp ../padded_conv_fp_tiled_loop_order_1.c ${TEMP}/temp.c
					fi

					rm tile_sizes.c
					echo "#define STRIDE_H ${stride}" >> tile_sizes.c
					echo "#define STRIDE_W ${stride}" >> tile_sizes.c
					echo "#define T_oi ${T_oi}" >> tile_sizes.c
					echo "#define T_oj ${T_oj}" >> tile_sizes.c
					echo "#define T_ifm_tile ${T_ifm_tile}" >> tile_sizes.c
					echo "#define T_ofm_tile ${T_ofm_tile}" >> tile_sizes.c
					cat temp/temp.c >> tile_sizes.c
					mv tile_sizes.c ${TEMP}/temp.c
					config_file=${config_num}_${images}_conv_config.txt
					output_file=${TEMP}/temp.c${config_file}_ws_stats.csv
					rm ${output_file}
					../../data_reuse_analyzer/polyscientist --input ${TEMP}/temp.c --config ${CONFIG_DIR}/${config_file} --minout 
					{ echo -n "${version}_${T_oi}_${T_oj}_${T_ifm_tile}_${T_ofm_tile},${GFLOPS}," ;  cat - ${output_file} ; } >> ${CONFIG_OUT}
					echo  "${NAIVE_GFLOPS},${ERROR}" >> ${META_CONFIG_OUT}


				fi
				done
				fi
				done
				fi
				done
				fi
				done
				echo
			else
				(cd .. && make clean && make MACROFLAGS="-DSTRIDE_H=$stride -DSTRIDE_W=$stride") 	
				../conv2d $config ${images} ${version} ${check_correctness} &> run_output
				GFLOPS=`cat run_output |  grep Real_GFLOPS |  cut -d= -f2`
				NAIVE_GFLOPS=`cat run_output |  grep Naive_GFLOPS |  cut -d= -f2`
				ERROR=`cat run_output | grep "inf-norm of comp. abs. error" | cut -d: -f 2`
                                rm ${TEMP}/temp.c
                                echo "#define STRIDE_H ${stride}" >> ${TEMP}/temp.c
                                echo "#define STRIDE_W ${stride}" >> ${TEMP}/temp.c
                                if [ $version -eq 2 ] || [ $version -eq 22 ]
                                then
                                cat ../padded_conv_fp_libxsmm_core.c >> ${TEMP}/temp.c
                                fi

                                if [ $version -eq 3 ] || [ $version -eq 23 ]
                                then
                                cat ../padded_conv_fp_libxsmm_core2.c >> ${TEMP}/temp.c
                                fi

                                if [ $version -eq 4 ] || [ $version -eq 24 ]
                                then
                                cat ../padded_conv_fp_libxsmm_core3.c >> ${TEMP}/temp.c
                                fi

                                if [ $version -eq 5 ] || [ $version -eq 25 ]
                                then
                                cat ../padded_conv_fp_libxsmm_core4.c >> ${TEMP}/temp.c
                                fi


                                if [ $version -eq 26 ]
                                then
                                cat ../padded_conv_fp5.c >> ${TEMP}/temp.c
                                fi

                                if [ $version -eq 27 ]
                                then
                                cat ../padded_conv_fp6.c >> ${TEMP}/temp.c
                                fi

                                if [ $version -eq 28 ]
                                then
                                cat ../padded_conv_fp7.c >> ${TEMP}/temp.c
                                fi

                                if [ $version -eq 29 ]
                                then
                                cat ../padded_conv_fp8.c >> ${TEMP}/temp.c
                                fi

                                if [ $version -eq 101 ]
                                then
                                cat ../padded_conv_fp_orig.c >> ${TEMP}/temp.c
                                fi

                                config_file=${config_num}_${images}_conv_config.txt
                                output_file=${TEMP}/temp.c${config_file}_ws_stats.csv
                                rm ${output_file}
                                ../../data_reuse_analyzer/polyscientist --input ${TEMP}/temp.c --config ${CONFIG_DIR}/${config_file} --minout
                                { echo -n "${version},${GFLOPS}," ; cat - ${output_file} ; } >> ${CONFIG_OUT}
				echo  "${NAIVE_GFLOPS},${ERROR}" >> ${META_CONFIG_OUT}

			fi
		done
		../../scripts/polyrank ${CONFIG_OUT}  --noheader --perfseparaterow --usepessidata
	done
	((config_num++))
done



