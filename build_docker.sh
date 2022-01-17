#!/bin/sh

set -e

sh scripts/compile.sh

GIT_BRANCH=`git rev-parse --abbrev-ref HEAD`
GIT_COMMIT=`git rev-parse --short HEAD`

TAG_VERSION=$GIT_BRANCH-$GIT_COMMIT

docker_name="ccr.ccs.tencentyun.com/tcb-100023372801-olry/prod-4gam91yoc446aa31_golang-o8a4:$TAG_VERSION"

docker build -t ${docker_name} ./

echo "Built ${docker_name} success"