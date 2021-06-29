rm -rf mkl-dnn
git clone https://github.com/oneapi-src/oneDNN.git mkl-dnn
cd mkl-dnn
git checkout tags/v2.2.1
mkdir -p build install
cd build
cmake -DCMAKE_INSTALL_PREFIX=../install/ ..
make -j50
make doc
make install
