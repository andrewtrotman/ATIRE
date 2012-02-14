#!/bin/bash

base=`dirname $0`
java -Djava.library.path="/usr/local/lib" -cp $base/bin org.atire.Indexer "$@" 