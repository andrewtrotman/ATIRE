#!/bin/bash

#!/bin/bash

home=`dirname $0`
home=`dirname $home`

echo "home:" $home

out=libatire_fat.a

current=`pwd`
cd $home/bin

lipo -create libatire_api_* -output $out


