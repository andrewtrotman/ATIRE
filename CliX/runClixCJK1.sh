#!/bin/bash

CLIX_PATH=.

if [ -n $CORPUS_HOME ]
then
    corpus_home=$CORPUS_HOME
else
    corpus_home=/data/corpus
fi

echo "CORPUS HOME: $corpus_home"

cp=
arguments=
with_arguments_remote=0
with_arguments_allnew=0;
with_arguments_extra=0;

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

arguments="-lang=zh|ja|ko|en -articlefile=${corpus_home}/wikipedia/CJK/xml/*articles*.txt -redirfile=${corpus_home}/wikipedia/CJK/xml/*redirections*.txt -outputdir=${corpus_home}/wikipedia/CJK/xml ${corpus_home}/wikipedia/CJK/zhwiki-20100627-pages-articles.xml.bz2 ${corpus_home}/wikipedia/CJK/jawiki-20100624-pages-articles.xml.bz2 ${corpus_home}/wikipedia/CJK/kowiki-20100628-pages-articles.xml.bz2"

arguments_remote="-Xdebug -Xrunjdwp:transport=dt_socket,address=8000,server=y"


while getopts 'r:n:e:' OPTION
	do
	  case $OPTION in
	  r)	with_arguments_remote=1
	  		arguments="$arguments_remote $arguments"
			;;
	  n)	with_arguments_allnew=1
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
#  arguments=$1
#fi
java -server -d64 -Xms1256m -Xmx16g -cp $cp:$CLIX_PATH/bin de.mpii.clix.wikipedia.Wiki2XML $arguments
