#!/bin/sh

docker build \
  --tag grafana-xxl \
  --build-arg GRAFANA_DEB_URL=https://github.com/fg2it/grafana-on-raspberry/releases/download/v5.0.4/grafana_5.0.4_armhf.deb \
  --build-arg GOSU_BIN_URL=https://github.com/tianon/gosu/releases/download/1.10/gosu-armhf .
