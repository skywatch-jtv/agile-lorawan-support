#!/usr/bin/env bash

FOLDER=$(dirname $(readlink -f $0))

GATEWAY_IMAGE=sky-watch/rpi-ttn-gateway

echo "Building image: ${GATEWAY_IMAGE}"
docker build --rm -t ${GATEWAY_IMAGE} -f ${FOLDER}/rpi-ttn-gateway/Dockerfile ${FOLDER}/rpi-ttn-gateway
