git clone --recursive https://github.com/apache/incubator-tvm tvm
cd tvm
git checkout dbd0114011906fe713ddda599bcba33a33b1fb73
#git checkout v0.6.0
git submodule init
git submodule update
mkdir build
cp cmake/config.cmake build
cd build
sed -i 's/set(USE_LLVM OFF)/set(USE_LLVM ON)/g' config.cmake
cmake .. -G Ninja
ninja
pip install numpy decorator attrs
pip install tornado xgboost
pip install cython
cd ..
pip install psutil
cd python; python3 setup.py install; cd ..
cd topi/python; python3 setup.py install; cd ../..
#(sudo apt-get install python3.5-dev)
