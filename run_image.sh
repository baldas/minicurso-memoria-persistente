#!/bin/bash

SCRIPT=`realpath $0`
SCRIPTPATH=`dirname $SCRIPT`

docker run --hostname docker-pmem --rm -ti -e HOST_USER_ID=$(id -u) -e HOST_USER_GID=$(id -g) -v "$SCRIPTPATH:/home/erad" francesquini/erad_pmem
