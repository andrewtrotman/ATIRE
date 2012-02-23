#!/bin/bash


swig -java -c++ -package org.atire.swig atire.swig

if ! [ -e "src/org/atire/swig/" ]
then
	mkdir -p src/org/atire/swig/
fi

\rm src/org/atire/swig/*
mv *.java src/org/atire/swig/

for i in `ls *.cxx`
do
	name=`echo $i | cut -f 1 -d "."`
	cp $i $name.c
done