#!/bin/bash

FOLDER=$(dirname $(readlink -f $0))
TAG=ttn_compiler

docker build --rm -t ${TAG} -f ${FOLDER}/Dockerfile ${FOLDER}
