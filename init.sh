#!/bin/bash


NGINX_VERSION=1.17.9
if [[ ! -d nginx-$NGINX_VERSION ]]; then
  curl -L https://nginx.org/download/nginx-$NGINX_VERSION.tar.gz -o nginx-$NGINX_VERSION.tar.gz
  tar -xzvf nginx-$NGINX_VERSION.tar.gz
fi

if [[ $(uname -s) == "Linux" ]]; then
  apt update
  apt install -y libssl-dev jq curl libcurl4{,-openssl-dev} vim build-essential libpcre3 libpcre3-dev zlib1g-dev
  if [[ -z $(which docker) ]]; then
    curl -fsSL https://get.docker.com -o get-docker.sh
    sh get-docker.sh
    usermod -aG docker $(id -un)
  fi
fi

cd nginx-$NGINX_VERSION
alias nginx=/usr/local/nginx/sbin/nginx
./configure --with-ld-opt="-lcurl" --with-cc-opt="-lcurl -I ../docker-nginx/src" --add-dynamic-module=../docker-nginx $( nginx -V 2>&1 | grep arguments | sed -e 's/configure arguments://g' | sed -e 's/-O2//g' | sed -e 's/fdebug-prefix-map=\/data\/builder\/debuild\/nginx-1.17.9\/debian\/debuild-base\/nginx-1.17.9=.\//g')
make modules

mkdir -p /usr/local/nginx/modules
cp objs/ngx_http_hello_world_module.so /usr/local/nginx/modules/

cd /usr/local/nginx/sbin
./nginx -s reload
if [[ $? == 0 ]]; then
  ./nginx
fi
