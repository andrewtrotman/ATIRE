#! /bin/bash

Q_OR_T=$1
PRECISION_MEASURE=$2

echo $Q_OR_T
echo $PRECISION_MEASURE

if [ "$Q_OR_T" != "QaaT" ] && [ "$Q_OR_T" != "TaaT" ]  && [ "$Q_OR_T" != "TaaT-partial" ]; then
	echo "please specify either QaaT or TaaT"
	exit
fi

py_script="$HOME/source-devel/hg/atire/experiments/quantum-at-a-time/benchmark.py"

collection_dir="$HOME/collections"
assessment_file="${collection_dir}/2009-INEX-Wikipedia/2010.qrels"
query_file="${collection_dir}/2009-INEX-Wikipedia/2010.topics"

CURRENT_DIR=`pwd`

#
# cd to ATIRE source directory and compile
#
cd "$HOME/source-devel/hg/atire"
make clean
make -f ./experiments/quantum-at-a-time/extra-info/GNUmakefile-${Q_OR_T} -j 4

#
# cd to the directory where it get called
#
cd $CURRENT_DIR

if [ "$Q_OR_T" = "QaaT" ]; then
	strategies=( M Pq:n Pq:d Pq:s Pq:l )
elif [ "$Q_OR_T" = "TaaT" ] || [ "$Q_OR_T" = "TaaT-partial" ]; then
	strategies=( M )
fi

#
# run the benchmark for the small range of top k
#
for st in "${strategies[@]}"
do
	cmd="python ${py_script} --ps ${st} -a ${assessment_file} -q ${query_file} --outfile output-small-${st}.csv --range small --repeat 1 --precision-measure ${PRECISION_MEASURE}"
	echo $cmd
	$cmd
done

#
# run the benchmark fro the large range of top k
#
for st in "${strategies[@]}"
do
	cmd="python ${py_script} --ps ${st} -a ${assessment_file} -q ${query_file} --outfile output-large-${st}.csv --range large --repeat 1 --precision-measure ${PRECISION_MEASURE}"
	echo $cmd
	$cmd
done