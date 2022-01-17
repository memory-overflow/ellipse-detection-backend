export LD_LIBRARY_PATH=./lib


port="8000"
log_dir="./logs"
image_dir="./images"

if [[ ${ENV_LOG_DIR} != "" ]]; then
  log_dir=${ENV_LOG_DIR}
fi

if [[ ${ENV_PORT} != "" ]]; then
  port=${ENV_PORT}
fi

if [[ ${ENV_IMAGE_DIR} != "" ]]; then
  image_dir=${ENV_IMAGE_DIR}
fi

mkdir -p ${log_dir}

./bin/server --flagfile=conf/ellipse_detection_flag.conf --log_dir=${log_dir} \
  --image_save_dir=${image_dir} --port=${port}