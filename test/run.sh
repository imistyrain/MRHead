if [ ! -d build ]; then
    mkdir build
fi
cd build
cmake ..
make -j4
export CPUPROFILE=test.prof
./test
#make install
pprof  ./test test.prof --web