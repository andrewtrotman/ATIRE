#! /bin/bash

Q_OR_T=$1

echo $Q_OR_T

if [ "$Q_OR_T" != "QaaT" ] && [ "$Q_OR_T" != "TaaT" ]; then
	echo "please specify either QaaT or TaaT"
	exit
fi

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
make -f ./experiments/quantum-at-a-time/GNUmakefile-${Q_OR_T}

#
# cd to the directory where it get called
#
cd $PWD

if [ "$Q_OR_T" = "QaaT" ]; then
	strategies=( M Pq:n Pq:d Pq:s Pq:l )
elif [ "$Q_OR_T" = "TaaT" ]; then
	strategies=( M )
fi

#
# run the benchmark for the small range of top k
#
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