mkdir third_party
cd third_party
wget http://barvinok.gforge.inria.fr/barvinok-0.41.5.tar.bz2
tar -xf barvinok-0.41.5.tar.bz2
wget https://libntl.org/ntl-11.3.2.tar.gz
tar -xf ntl-11.3.2.tar.gz
mkdir ntl-11.3.2_install
cd ntl-11.3.2/src/
./configure PREFIX=${PWD}/../../ntl-11.3.2_install NTL_GMP_LIP=on
make
make install
cd ../..
mkdir barvinok-0.41.5_install
cd barvinok-0.41.5
source /swtools/clang/12.0.0/vars.sh
./configure --prefix=${PWD}/../barvinok-0.41.5_install --with-ntl-prefix=${PWD}/../ntl-11.3.2_install --with-pet=bundled 
make
make install
cd ../../
