#!/bin/bash
TTN_TAG=${1:-${TTN_GIT_VERSION:-master}}
FOLDER=$(dirname $(readlink -f $0))

ARCH=$(uname -m)
SHORT_ARCH=$(echo ${ARCH} | sed 's/armv[0-9]\+.*/arm/g')

TTN_IMAGE=ttn-${ARCH}:${TTN_TAG}
DISCOVERY_IMAGE=sky-watch/ttn-${ARCH}-discovery:${TTN_TAG}
ROUTER_IMAGE=sky-watch/ttn-${ARCH}-router:${TTN_TAG}
NETWORKSERVER_IMAGE=sky-watch/ttn-${ARCH}-networkserver:${TTN_TAG}

BROKER_IMAGE=sky-watch/ttn-${ARCH}-broker:${TTN_TAG}
HANDLER_IMAGE=sky-watch/ttn-${ARCH}-handler:${TTN_TAG}

IMAGES_FOLDER=${FOLDER}/ttn-images

#
# TTN-BASE-IMAGE
#
if [ -z $(docker images -q "${TTN_IMAGE}") ]; then
  ${FOLDER}/ttn-base-image/build.sh ${TTN_TAG}
fi

# prep folders
mkdir -p ${IMAGES_FOLDER}/generated

#
# DISCOVERY
#
echo -e "\nBuilding image: ${DISCOVERY_IMAGE}"
docker build --rm -t ${DISCOVERY_IMAGE} -f ${IMAGES_FOLDER}/Dockerfile.discovery --build-arg TTN_IMAGE=${TTN_IMAGE} ${IMAGES_FOLDER}

echo -e "\nDiscovery server certificate"
docker run --rm --entrypoint /bin/cat ${DISCOVERY_IMAGE} "/etc/ttn/discovery/server.cert" &>${IMAGES_FOLDER}/generated/discovery.cert
cat ${IMAGES_FOLDER}/generated/discovery.cert

echo -e "\nDiscovery Router Access Token"
DISCOVERY_ROUTER_ACCESS_TOKEN=$(docker run --rm ${DISCOVERY_IMAGE} discovery authorize router agile-router --config /etc/ttn/discovery/ttn.yml | tail -2 | head -1)
echo ${DISCOVERY_ROUTER_ACCESS_TOKEN}

echo -e "\nDiscovery Broker Access Token"
DISCOVERY_BROKER_ACCESS_TOKEN=$(docker run --rm ${DISCOVERY_IMAGE} discovery authorize broker agile-broker --config /etc/ttn/discovery/ttn.yml | tail -2 | head -1)
echo ${DISCOVERY_BROKER_ACCESS_TOKEN}

echo -e "\nDiscovery Handler Access Token"
DISCOVERY_HANDLER_ACCESS_TOKEN=$(docker run --rm ${DISCOVERY_IMAGE} discovery authorize handler agile-handler --config /etc/ttn/discovery/ttn.yml | tail -2 | head -1)
echo ${DISCOVERY_HANDLER_ACCESS_TOKEN}

#
# ROUTER
#
echo -e "\nBuilding image: ${ROUTER_IMAGE}"
cat ${IMAGES_FOLDER}/config/router.yml | \
sed s/$\{DISCOVERY_ROUTER_ACCESS_TOKEN}/${DISCOVERY_ROUTER_ACCESS_TOKEN}/g >${IMAGES_FOLDER}/generated/router.yml
docker build --rm -t ${ROUTER_IMAGE} -f ${IMAGES_FOLDER}/Dockerfile.router --build-arg TTN_IMAGE=${TTN_IMAGE} ${IMAGES_FOLDER}

#
# NETWORK SERVER
#
echo -e "\nBuilding image: ${NETWORKSERVER_IMAGE}"
docker build --rm -t ${NETWORKSERVER_IMAGE} -f ${IMAGES_FOLDER}/Dockerfile.networkserver --build-arg TTN_IMAGE=${TTN_IMAGE} ${IMAGES_FOLDER}

echo -e "\nNetworkserver server certificate"
docker run --rm --entrypoint /bin/cat ${NETWORKSERVER_IMAGE} "/etc/ttn/networkserver/server.cert" &>${IMAGES_FOLDER}/generated/networkserver.cert
cat ${IMAGES_FOLDER}/generated/networkserver.cert

echo -e "\nNetworkserver Broker Access Token"
NETWORKSERVER_BROKER_ACCESS_TOKEN=$(docker run --rm ${NETWORKSERVER_IMAGE} networkserver authorize agile-broker --config /etc/ttn/networkserver/ttn.yml | tail -2 | head -1)
echo ${NETWORKSERVER_BROKER_ACCESS_TOKEN}

#
# BROKER
#
echo -e "\nBuilding image: ${BROKER_IMAGE}"
cat ${IMAGES_FOLDER}/config/broker.yml | \
sed s/$\{DISCOVERY_BROKER_ACCESS_TOKEN}/${DISCOVERY_BROKER_ACCESS_TOKEN}/g | \
sed s/$\{NETWORKSERVER_BROKER_ACCESS_TOKEN}/${NETWORKSERVER_BROKER_ACCESS_TOKEN}/g >${IMAGES_FOLDER}/generated/broker.yml
docker build --rm -t ${BROKER_IMAGE} -f ${IMAGES_FOLDER}/Dockerfile.broker --build-arg TTN_IMAGE=${TTN_IMAGE} ${IMAGES_FOLDER}

#
# HANDLER
#
echo -e "\nBuilding image: ${HANDLER_IMAGE}"
cat ${IMAGES_FOLDER}/config/handler.yml | \
sed s/$\{DISCOVERY_HANDLER_ACCESS_TOKEN}/${DISCOVERY_HANDLER_ACCESS_TOKEN}/g >${IMAGES_FOLDER}/generated/handler.yml
docker build --rm -t ${HANDLER_IMAGE} -f ${IMAGES_FOLDER}/Dockerfile.handler --build-arg TTN_IMAGE=${TTN_IMAGE} ${IMAGES_FOLDER}

#
# BRIDGE
#
echo
${FOLDER}/ttn-gateway-connector-bridge/build.sh
