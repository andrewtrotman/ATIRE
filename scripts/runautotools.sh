#!/bin/bash

aclocal
autoconf
libtoolize
touch NEWS README AUTHORS ChangeLog
automake --add-missing
