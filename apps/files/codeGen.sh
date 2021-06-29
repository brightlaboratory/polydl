#!/bin/bash    
mkdir file_data;
echo "Part 1";
`grep -EHrin "T_oi|T_oj|T_ofm_tile|T_ifm_tile" fwd_perf_N_1.txt | cut -d"=" -f2 > temp`;
`grep -EHrin "T_oi|T_oj|T_ofm_tile|T_ifm_tile" fwd_perf_N_28.txt | cut -d"=" -f2 >> temp`;

for f in $(seq 1 38);
do
       	cp ../padded_conv_fp_stride_1_tiled_loop_order_0.c ./file_data/padded_conv_fp_stride_1_tiled_loop_order_0_$f.c;
	sed -i '1,4d' ./file_data/padded_conv_fp_stride_1_tiled_loop_order_0_$f.c;
	sed -i '1i #define T_oi='"$(sed -n 1p temp)" ./file_data/padded_conv_fp_stride_1_tiled_loop_order_0_$f.c;
	sed -i '1d' temp;
	
	sed -i '2i #define T_oj='"$(sed -n 1p temp)" ./file_data/padded_conv_fp_stride_1_tiled_loop_order_0_$f.c;
	sed -i '1d' temp;
	
	sed -i '3i #define T_ofm_tile='"$(sed -n 1p temp)" ./file_data/padded_conv_fp_stride_1_tiled_loop_order_0_$f.c;
	sed -i '1d' temp;
	
	sed -i '4i #define T_ifm_tile='"$(sed -n 1p temp)" ./file_data/padded_conv_fp_stride_1_tiled_loop_order_0_$f.c;
	sed -i '1d' temp;

done;

echo "Part 2";
`grep -EHrin "T_oi|T_oj|T_ofm_tile|T_ifm_tile" fwd_perf_N_1.txt | cut -d"=" -f2 > temp`;
`grep -EHrin "T_oi|T_oj|T_ofm_tile|T_ifm_tile" fwd_perf_N_28.txt | cut -d"=" -f2 >> temp`;

for f in $(seq 1 38);
do
       	cp ../padded_conv_fp_stride_1_tiled_loop_order_1.c ./file_data/padded_conv_fp_stride_1_tiled_loop_order_1_$f.c;
	sed -i '4,7d' ./file_data/padded_conv_fp_stride_1_tiled_loop_order_1_$f.c;
	sed -i '1i #define T_oi='"$(sed -n 1p temp)" ./file_data/padded_conv_fp_stride_1_tiled_loop_order_1_$f.c;
	sed -i '1d' temp;
	
	sed -i '2i #define T_oj='"$(sed -n 1p temp)" ./file_data/padded_conv_fp_stride_1_tiled_loop_order_1_$f.c;
	sed -i '1d' temp;
	
	sed -i '3i #define T_ofm_tile='"$(sed -n 1p temp)" ./file_data/padded_conv_fp_stride_1_tiled_loop_order_1_$f.c;
	sed -i '1d' temp;
	
	sed -i '4i #define T_ifm_tile='"$(sed -n 1p temp)" ./file_data/padded_conv_fp_stride_1_tiled_loop_order_1_$f.c;
	sed -i '1d' temp;

done;
rm temp;
