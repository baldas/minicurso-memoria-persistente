#!/bin/bash

SCRIPT=`realpath $0`
SCRIPTPATH=`dirname $SCRIPT`

docker build --tag "erad_pmem" -f "$SCRIPTPATH/Dockerfile" "$SCRIPTPATH"
