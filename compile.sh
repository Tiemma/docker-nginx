#!/bin/bash

NGINX_VERSION=1.17.9
cd nginx-$NGINX_VERSION
./configure --with-debug --with-cc-opt='-O0 -g' $( nginx -V 2>&1 | grep arguments | sed -e 's/configure arguments://g' | sed -e 's/-O2//g' | sed -e 's/fdebug-prefix-map=\/data\/builder\/debuild\/nginx-1.17.9\/debian\/debuild-base\/nginx-1.17.9=.\//g')
make
make install
ln -s /usr/local/nginx/sbin/nginx $(which nginx)


