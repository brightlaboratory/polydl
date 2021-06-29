
#sample usage: sh copy_files_to_another_directory.sh resnet8_10
DIR=$1
mkdir ../$DIR
cp *.c ../$DIR/
cp Makefile ../$DIR/
cp *.sh ../$DIR/
mkdir ../$DIR/experiments
cp experiments/*.sh ../$DIR/experiments/
cp experiments/*.batch ../$DIR/experiments/
