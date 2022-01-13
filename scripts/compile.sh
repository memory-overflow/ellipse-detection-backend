#!/bin/sh
set -e

cur_path=$(cd `dirname $0`; pwd)
cd ${cur_path}/../

/usr/bin/protoc --cpp_out=./ proto/ellipse_detect.proto

rm -rf build pack

mkdir build && cd build
cmake ..
make
cd ..

mkdir pack
cp build/server pack
cp -r conf pack/