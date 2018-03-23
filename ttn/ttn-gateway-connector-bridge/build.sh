#!/bin/bash
TTN_TAG=v2.3.1
SRC_REPO=https://github.com/TheThingsNetwork/gateway-connector-bridge.git

FOLDER=$(dirname $(readlink -f $0))
WORKSPACE=${FOLDER}/workspace/${TTN_TAG}

ARCH=$(uname -m)
SHORT_ARCH=$(echo ${ARCH} | sed 's/armv[0-9]\+.*/arm/g')

BRIDGE_IMAGE=gateway-connector-bridge-${ARCH}:${TTN_TAG}

if [ ! -z $(docker images -q "${BRIDGE_IMAGE}") ]; then
  echo "Image already exists: ${BRIDGE_IMAGE}"
  exit 0
fi

COMPILER_IMAGE=ttn_compiler
if [ -z $(docker images -q "${COMPILER_IMAGE}") ]; then
  echo "Build compiler image for TTN on ${ARCH}"
  ${FOLDER}/../ttn_compiler/build.sh
fi

echo "Create workspace folder for the TTN gateway-connector-bridge source: ${WORKSPACE}"
mkdir -p ${WORKSPACE}

echo "Compiling gateway-connector-bridge on ${ARCH}, using repository ${SRC_REPO}"
TMP_SCRIPT=$(mktemp)
cat ${FOLDER}/compile_bridge.sh | 
sed s/$\{SRC_REPO}/$(echo ${SRC_REPO} | sed -e 's/[\/&]/\\&/g')/g | 
sed s/$\{REPO_TAG}/$(echo ${TTN_TAG} | sed -e 's/[\/&]/\\&/g')/g >${TMP_SCRIPT}
chmod +x ${TMP_SCRIPT}
docker run --rm -v "${WORKSPACE}:/work" -v "${TMP_SCRIPT}:/cmd.sh" ${COMPILER_IMAGE} "/cmd.sh" || exit 20
rm ${TMP_SCRIPT}

RELEASE_FOLDER=${WORKSPACE}/src/github.com/TheThingsNetwork/gateway-connector-bridge
echo "Building gateway-connector-bridge docker image: ${BRIDGE_IMAGE}"
cat ${RELEASE_FOLDER}/Dockerfile | sed -e "s/amd64/${SHORT_ARCH}/g" | docker build -f - -t ${BRIDGE_IMAGE} ${RELEASE_FOLDER}
