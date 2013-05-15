#!/bin/bash

home=`dirname $0`
home=`dirname $home`

echo "home:" $home

out=libatire_api_${arch}.a

echo "arch:" $arch

if ! [ -z "$arch" ]; then
	lipo -create -arch ${arch} .libs/libatire_api.a -output $out
	mv $out ${home}/bin        
else
	echo "arch" variable is not defined.
fi
