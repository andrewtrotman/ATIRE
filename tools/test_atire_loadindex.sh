#!/bin/sh

# Test out Atire's index loading capabilities by having it load a new
# index over and over again. This is to see if it ends up dying from leaks
	
# After creating doclist.aspt, index.aspt, doclist.aspt.2, index.aspt.2,  	
	
# Pipe the output of this script to atire

while true
do
	echo "<ATIREloadindex><doclist>doclist.aspt</doclist><index>index.aspt</index></ATIREloadindex>"
	echo "<ATIREloadindex><doclist>doclist.aspt.2</doclist><index>index.aspt.2</index></ATIREloadindex>"
done