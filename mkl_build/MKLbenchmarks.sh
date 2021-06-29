rm -rf perf.csv
cd mkl-dnn/build/
cp ../../run_bench_all.sh ./tests/benchdnn/
cd ./tests/benchdnn
sh run_bench_all.sh
cp ./perf.csv ../../../../
echo "success"
