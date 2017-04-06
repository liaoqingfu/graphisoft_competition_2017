#!/usr/bin/env bash

if ! docker images -a iskolabusz | grep iskolabusz > /dev/null 2>&1; then
    cd ubuntu && docker build -t iskolabusz .
fi

mdir=$(cd $(dirname $0) && pwd)
echo $mdir

# TODO: replace $PWD with the correct directory!
docker run -v $mdir:/mnt/stuff --name iskolabusz_container -it iskolabusz /bin/bash /mnt/stuff/cr.sh
docker stop iskolabusz_container
docker rm iskolabusz_container
