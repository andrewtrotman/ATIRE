#!/bin/sh

# Test out Atire's searching capabilities by having serve queries over and over

# Pipe the output of this script to atire

while true
do
	echo "<ATIREsearch><query>pure</query></ATIREsearch>"
	echo "<ATIREsearch><query>apple</query></ATIREsearch>"
	echo "<ATIREsearch><query>bananas</query></ATIREsearch>"
	echo "<ATIREsearch><query></query></ATIREsearch>"
	echo "<ATIREsearch><query>aeoifaev</query></ATIREsearch>"
done