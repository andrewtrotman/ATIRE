#! /bin/bash

py_script="/home/fei/source-devel/hg/atire/experiments/quantum-at-a-time/benchmark.py"

collection_dir="/home/fei/collections"
assessment_file="${collection_dir}/2009-INEX-Wikipedia/2010.qrels"
query_file="${collection_dir}/2009-INEX-Wikipedia/2010.topics"

strategies=( M Pq:n Pq:d Pq:s Pq:l )
for s in "${strategies[@]}"
do
	cmd="python ${py_script} --ps ${s} -a ${assessment_file} -q ${query_file} --outfile output-${s}.csv"
	echo $cmd
	$cmd
done