DIR=$1
NEWDIR=cleansed_${DIR}
parentdir="$(dirname "$DIR")"
lastdir="$(basename -- "$DIR")"
echo parentdir: $parentdir
echo newdir: $NEWDIR
echo lastdir: $lastdir
NEWDIR=$parentdir/cleansed_${lastdir}
echo NEWDIR: $NEWDIR
mkdir $NEWDIR
for file in $DIR/*; do 
   filename=$(basename -- "$file")
   echo $filename
   cat $file | grep -v 5_64 > $NEWDIR/$filename 
done
