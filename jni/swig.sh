#!/bin/bash


swig -java -c++ -package org.atire.swig atire.swig

if ! [ -e "src/org/atire/swig/" ]
then
	mkdir -p src/org/atire/swig/
fi

\rm src/org/atire/swig/*
mv *.java src/org/atire/swig/