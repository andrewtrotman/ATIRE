#! /bin/bash

py_script="$HOME/source-devel/hg/atire/experiments/quantum-at-a-time/benchmark.py"

collection_dir="$HOME/collections"
assessment_file="${collection_dir}/2009-INEX-Wikipedia/2010.qrels"
query_file="${collection_dir}/2009-INEX-Wikipedia/2010.topics"

PWD=`pwd`

#
# cd to ATIRE source directory and compile
#
cd "$HOME/source-devel/hg/atire"
make clean
make -f ./experiments/quantum-at-a-time/GNUmakefile-QaaT

#
# cd to the directory where it get called
#
cd $PWD

#
# run the benchmark for the small range of top k
#
strategies=( M Pq:n Pq:d Pq:s Pq:l )
for st in "${strategies[@]}"
do
	cmd="python ${py_script} --ps ${st} -a ${assessment_file} -q ${query_file} --outfile output-small-${s}.csv --range small"
	echo $cmd
	$cmd
done

#
# run the benchmark fro the large range of top k
#
for st in "${strategies[@]}"
do
	cmd="python ${py_script} --ps ${st} -a ${assessment_file} -q ${query_file} --outfile output-large-${s}.csv --range large"
	echo $cmd
	$cmd
done