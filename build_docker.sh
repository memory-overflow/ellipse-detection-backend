#!/bin/sh

set -e

sh scripts/compile.sh

GIT_BRANCH=`git rev-parse --abbrev-ref HEAD`
GIT_COMMIT=`git rev-parse --short HEAD`

TAG_VERSION=$GIT_BRANCH-$GIT_COMMIT

docker_name="ccr.ccs.tencentyun.com/tcb-100023083266-dimb/ellipse-detection:$TAG_VERSION"

docker build -t ${docker_name} ./

echo "Built ${docker_name} success"