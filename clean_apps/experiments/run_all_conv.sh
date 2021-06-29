#!/bin/bash

version_file=$1
FILE=run_conv.sh
bash run_resnet.sh $FILE $version_file
# bash run_fastrcnn.sh $FILE $version_file
# bash run_maskrcnn.sh $FILE $version_file
# bash run_xception.sh $FILE $version_file
# bash run_yolov2.sh $FILE $version_file
# bash run_mobilenet.sh $FILE $version_file
# bash run_alexnet.sh $FILE $version_file
# bash run_overfeat.sh $FILE $version_file
# bash run_googlenetv1.sh $FILE $version_file
# bash run_googlenetv3.sh $FILE $version_file
# bash run_vgga.sh $FILE $version_file
