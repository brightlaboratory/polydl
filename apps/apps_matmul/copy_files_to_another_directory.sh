
#sample usage: sh copy_files_to_another_directory.sh matmul_baseline
DIR=$1
NEWDIR=../../$DIR/apps_matmul
mkdir -p $NEWDIR
cp *.c $NEWDIR
cp -r versions $NEWDIR
cp Makefile $NEWDIR
cp *.sh $NEWDIR
mkdir $NEWDIR/experiments
cp experiments/*.sh $NEWDIR/experiments/
cp experiments/*.batch $NEWDIR/experiments/
