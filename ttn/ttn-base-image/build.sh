#!/bin/bash
TTN_TAG=${1:-${TTN_GIT_VERSION:-master}}
SRC_REPO=${2:-https://github.com/TheThingsNetwork/ttn.git}

FOLDER=$(dirname $(readlink -f $0))
WORKSPACE=${FOLDER}/workspace/${TTN_TAG}

ARCH=$(uname -m)
SHORT_ARCH=$(echo ${ARCH} | sed 's/armv[0-9]\+.*/arm/g')

TTN_IMAGE=ttn-${ARCH}:${TTN_TAG}

if [ ! -z $(docker images -q "${TTN_IMAGE}") ]; then
  echo "Image already exists: ${TTN_IMAGE}"
  exit 0
fi

COMPILER_IMAGE=ttn_compiler
if [ -z $(docker images -q "${COMPILER_IMAGE}") ]; then
  echo "Build compiler image for TTN on ${ARCH}"
  ${FOLDER}/../ttn_compiler/build.sh
fi

echo "Create workspace folder for the TTN source: ${WORKSPACE}"
mkdir -p ${WORKSPACE}

echo "Compiling TTN on ${ARCH}, using repository ${SRC_REPO}"
TMP_SCRIPT=$(mktemp)
cat ${FOLDER}/compile_ttn.sh | 
sed s/$\{SRC_REPO}/$(echo ${SRC_REPO} | sed -e 's/[\/&]/\\&/g')/g | 
sed s/$\{REPO_TAG}/$(echo ${TTN_TAG} | sed -e 's/[\/&]/\\&/g')/g >${TMP_SCRIPT}
chmod +x ${TMP_SCRIPT}
docker run --rm -v "${WORKSPACE}:/work" -v "${TMP_SCRIPT}:/cmd.sh" ${COMPILER_IMAGE} "/cmd.sh" || exit 20
rm ${TMP_SCRIPT}

RELEASE_FOLDER=${WORKSPACE}/src/github.com/TheThingsNetwork/ttn
echo "Building TTN docker image: ${TTN_IMAGE}"
cat ${RELEASE_FOLDER}/Dockerfile | sed -e "s/amd64/${SHORT_ARCH}/g" | docker build -f - -t ${TTN_IMAGE} ${RELEASE_FOLDER}
