#!/bin/bash

for file in ./version/*
# for file in padded_conv_fp_libxsmm_core2.c
do
	make clean
	cd experiments;
	bash run_all_conv.sh ${file}
	cd ..
done

