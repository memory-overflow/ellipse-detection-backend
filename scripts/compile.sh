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

packdir="pack"
mkdir -p ${packdir}/lib
mkdir -p ${packdir}/bin

cp build/server ${packdir}/bin
cp -r conf ${packdir}/
cp -r scripts ${packdir}/

dep_list=$( ldd "build/server" | awk '{if (match($3,"/")){ print $3 }}')
if [ ! -z "$dep_list" ]
then
    cp -L $dep_list ${packdir}/lib
fi