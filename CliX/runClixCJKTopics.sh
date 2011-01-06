#!/bin/bash

CLIX_PATH=/home/monfee/workspace/CliX

cp=

append_jars_to_cp() {
    # N.b.  This adds all the jars of a directory to $cp.
    # That may be ok in some other situation, but you probably should
    # NOT run "append_jars_to_cp $HSQLDB_HOME/lib" (for one thing, that
    # directory may contain non-inter-compatible libraries).
    dir="$1"
    for ex in jar zip ; do
        if [ "`echo ${dir}/*.$ex`" != "${dir}/*.$ex" ] ; then
            for x in ${dir}/*.$ex ; do
                if [ ! -z "$cp" ] ; then cp="$cp:" ; fi
                cp="$cp$x"
            done
        fi
    done
}

append_jars_to_cp $CLIX_PATH/lib

arguments="/data/corpus/wikipedia/CJK/zhwiki-20100627-pages-articles.xml.bz2 /data/corpus/wikipedia/CJK/jawiki-20100624-pages-articles.xml.bz2 /data/corpus/wikipedia/CJK/kowiki-20100628-pages-articles.xml.bz2"


while getopts ':r:n:e:' OPTION
	do
	  case $OPTION in
	  r)	with_arguments_remote=1
	  		arguments="$arguments_remote $arguments"
			;;
	  n)	with_arguments_allnew=1
	  		#echo "$OPTARG"
			arguments="$OPTARG"
			;;
	  e)	with_arguments_extra=1
	  		arguments="$arguments $OPTARG"
			;;			
	  ?)	printf "Usage: %s: [-r] [-n] [-e]\n" $(basename $0) >&2
			exit 2
			;;
	  esac
done

#if [ -n "$1" ]
#then
#echo "arguments:  $arguments"
#fi
java  -Xms256m -Xmx3280m -cp $cp:$CLIX_PATH/bin de.mpii.clix.wikipedia.CJKTopicRecommender $arguments
