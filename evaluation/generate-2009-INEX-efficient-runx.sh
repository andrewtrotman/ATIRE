#!/bin/bash

#
# need root privilige to run
#

function flush_cache() {
	sync
	# free pagecache
	# echo 1 > /proc/sys/vm/drop_caches
	# free dentries and inodes
	# echo 2 > /proc/sys/vm/drop_caches
	# free pagecache, dentries and inodes
	echo 3 > /proc/sys/vm/drop_caches
	if [ $? -gt 0 ] ; then
		echo "ERROR: calling flush_cache()";
		exit
	fi
}


# the global run_id counter
run_id=1

ID_PREFIX="Eff-"
QUERIES="./efficiency-topics/2009-efficiency-topics.txt"

function generate_runs() {
	topk=$1;
	impact_orderings="1 15 150 1500 15000 150000 1500000";

	#
	# topk = 15
	#
	for impact in $impact_orderings; do
		flush_cache
		filename="OTAGO-DOC-k$topk-K$impact-$ID_PREFIX$run_id"
		echo /home/fei/source-devel/ant/bin/ant -ee -sa -k$topk -K$impact -i4 -n$ID_PREFIX$run_id -l$topk -o$filename.xml
		/home/fei/source-devel/ant/bin/ant -ee -sa -k$topk -K$impact -i4 -n$ID_PREFIX$run_id -l$topk -o$filename.xml < $QUERIES
		zip $filename $filename.xml
 		let run_id++
	done

}


#
# The main starts here
#
generate_runs 15
generate_runs 150
generate_runs 1500

chown fei:fei OTAGO-DOC-*

