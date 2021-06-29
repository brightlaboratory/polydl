OUT=perf.csv
rm ${OUT}

#Default values.
iters=1
ifw=56
ifh=56
nIfm=64
nOfm=256
kw=1
kh=1
pad_w=0
pad_h=0
stride=1

mb=28
nImg=1
#Taking values from terminal

iters=$1
ifw=$2
ifh=$3
nIfm=$4
nOfm=$5
kw=$6
kh=$7
pad_w=$8
pad_h=$9
stride=${10}
config_num=${11}
filename=${12}

stride_w=$stride
stride_h=$stride

pad_h_in=0
pad_w_in=0
pad_h_out=0
pad_w_out=0

ofh=$(((ifh + 2 * pad_h - kh) / stride_h + 1));
ofw=$(((ifw + 2 * pad_w - kw) / stride_w + 1));
ifhp=$((ifh + 2 * pad_h_in));
ifwp=$((ifw + 2 * pad_w_in));
ofhp=$((ofh + 2 * pad_h_out));
ofwp=$((ofw + 2 * pad_w_out));


config="nImg=${nImg} ifh=${ifh} ifw=${ifw} ifhp=${ifhp} ifwp=${ifwp} ofh=${ofh} ofw=${ofw} ofhp=${ofhp} ofwp=${ofwp} nIfm=${nIfm} nOfm=${nOfm} kh=${kh} kw=${kw} pad_h=${pad_h} pad_w=${pad_w} stride_h=${stride_h} stride_w=${stride_w} pad_h_in=${pad_h_in} pad_w_in=${pad_w_in} pad_w_out=${pad_w_out} pad_h_out=${pad_h_out}"


execute=`./haystack -c  32768 1048576 1441792 -d $config -f ${filename}`
GFLOPS_Com=` echo "${execute}" | grep 'compulsory' | cut -d: -f2 `
compulsory_miss=`echo $GFLOPS_Com | sed 's/,//g'`
GFLOPS=` echo "${execute}" | grep 'capacity' | cut -d: -f2 `
capacity_miss=`echo $GFLOPS_Com | sed 's/,//g'`

# GFLOPS=`./haystack -c  32768 1048576 1441792 -d $config -f ${filename} | grep 'capacity\|compulsory' | cut -d: -f2`
#GFLOPS=`./conv2d $config ${images} ${version} |  grep GFLOPS |  cut -d= -f2`
#GFLOPS=`./conv2d $config ${images} ${version}`

echo -n ${config_num},$compulsory_miss,$compulsory_miss,$compulsory_miss,$capacity_miss, >> ${OUT}
echo "" >> ${OUT}

