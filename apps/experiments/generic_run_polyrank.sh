
DIR=$1
NEWDIR=backup_${DIR}
mkdir $NEWDIR
for file in $DIR/*; do
   ../../scripts/polyrank $file --noheader --perfseparaterow --usepessidata
done


