#/bin/bash
if [ ! -d build ]; then
    mkdir build
fi
cd build

#DEMO_NAME=glcv
DEMO_NAME=gltest

cmake .. -DPROJECT_NAME=${DEMO_NAME}
make -j4
./${DEMO_NAME}