#!/bin/bash

if [ ! -e "src/github.com/TheThingsNetwork/ttn" ]; then
  git clone ${SRC_REPO} src/github.com/TheThingsNetwork/ttn
  cd src/github.com/TheThingsNetwork/ttn
else
  cd src/github.com/TheThingsNetwork/ttn
  ORIGIN=$(git config --get remote.origin.url)
  echo "Origin: ${ORIGIN}"
  echo "Compare with: ${SRC_REPO}"
  if [ "${SRC_REPO}" == "${ORIGIN}" ]; then
    git pull
  else
    (>&2 echo "Existing repository is NOT from the same origin")
    exit 20
  fi
fi

git checkout ${REPO_TAG}
make dev-deps
make test
make build

