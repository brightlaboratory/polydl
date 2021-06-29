
PERF_DIR=perf_data
OUT=poly_perf.csv

for config_num in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19
  do
    for images in 1 28
      do
        CONFIG_OUT=${PERF_DIR}/${config_num}_${images}_${OUT}
 	../../scripts/polyrank ${CONFIG_OUT} --noheader --perfseparaterow --usepessidata 
      done
done

