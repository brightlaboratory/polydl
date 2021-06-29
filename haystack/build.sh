git clone https://github.com/spcl/haystack.git
cd haystack
mkdir build
./get_submodules.sh
./autogen.sh
rm -rf pet
git clone https://repo.or.cz/pet.git
cd pet
./get_submodules.sh
./autogen.sh
cd ..
cd build
../configure
make -j12
