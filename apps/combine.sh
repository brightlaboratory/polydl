cut -d ',' -f1 padded_conv_fp_stride_1_tiled_loop_order_0.cconv_config.txt_ws_stats.csv >temp.csv;
cut -d"," -f3 perf.csv | sed -n '1,20p' >temp1.csv;
cut -d ',' -f2- padded_conv_fp_stride_1_tiled_loop_order_0.cconv_config.txt_ws_stats.csv >temp2.csv;
cut -d "," -f3 perf.csv |sed -n '1p;21,39p' >temp3.csv;
cut -d ',' -f2- padded_conv_fp_stride_1_tiled_loop_order_1.cconv_config.txt_ws_stats.csv >temp4.csv;
paste -d "," temp.csv temp1.csv temp2.csv temp3.csv temp4.csv >out.csv;
rm temp*;
awk -F, '{$2=0 FS $2;}1' OFS=, out.csv>temp.csv;
sed '/^params,/s/[^,]*/Version/2' temp.csv>out.csv;
awk -F, '{$12=1 FS $12;}1' OFS=, out.csv>temp.csv;
sed '/^params,/s/[^,]*/Version/12' temp.csv>out.csv;
rm temp.csv;
mv out.csv out_N_1.csv

cut -d ',' -f1 padded_conv_fp_stride_1_tiled_loop_order_0.cconv_config_N_28.txt_ws_stats.csv >temp.csv;
cut -d"," -f3 perf.csv | sed -n '1p;40,58p' >temp1.csv;
cut -d ',' -f2- padded_conv_fp_stride_1_tiled_loop_order_0.cconv_config_N_28.txt_ws_stats.csv >temp2.csv;
cut -d "," -f3 perf.csv |sed -n '1p;59,77p' >temp3.csv;
cut -d ',' -f2- padded_conv_fp_stride_1_tiled_loop_order_1.cconv_config_N_28.txt_ws_stats.csv >temp4.csv;
paste -d "," temp.csv temp1.csv temp2.csv temp3.csv temp4.csv >out.csv;
rm temp*;
awk -F, '{$2=0 FS $2;}1' OFS=, out.csv>temp.csv;
sed '/^params,/s/[^,]*/Version/2' temp.csv>out.csv;
awk -F, '{$12=1 FS $12;}1' OFS=, out.csv>temp.csv;
sed '/^params,/s/[^,]*/Version/12' temp.csv>out.csv;
rm temp.csv;
mv out.csv out_N_28.csv

