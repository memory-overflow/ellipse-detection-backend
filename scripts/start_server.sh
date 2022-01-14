export LD_LIBRARY_PATH=./lib

mkdir -p ./logs

./bin/server --flagfile=conf/ellipse_detection_flag.conf &
