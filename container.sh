#!/bin/bash

if [ -z $(which docker) ]; then
  echo "Install docker"
  exit
fi

NETWORK=nginx
docker network create $NETWORK
docker run -it -e NETWORK=$NETWORK -v $PWD:/src -v /var/run/docker.sock:/var/run/docker.sock --network $NETWORK --name nginx -d nginx
docker run -it -e PORT=80 --network $NETWORK --name rest -d vad1mo/hello-world-rest

if [[ ! $? == 0 ]]; then
  docker start nginx rest
fi
