#!/bin/bash
if [ ! -d build ]; then
    mkdir build
fi
cd build
cmake ..
make -j4
./main1 > image.ppm