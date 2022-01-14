FROM centos:7

COPY ./pack /usr/local/services/ellipse-detection

RUN echo "export LANG=en_US.UTF-8" >> /etc/bashrc && echo "Asia/shanghai" >> /etc/timezone

WORKDIR /usr/local/services/ellipse-detection

CMD ["sh", "-c", "sh scripts/start_server.sh; while true; do sleep 99999999; done"]