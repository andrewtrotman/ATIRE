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

arguments="-articlefile=/data/corpus/wikipedia/CJK/xml/*articles.txt -redirfile=/data/corpus/wikipedia/CJK/xml/*redirections.txt -outputdir=/data/corpus/wikipedia/CJK/xml /data/corpus/wikipedia/CJK/zhwiki-20091119-pages-articles.xml.bz2 /data/corpus/wikipedia/CJK/jawiki-20100624-pages-articles.xml.bz2 /data/corpus/wikipedia/CJK/kowiki-20100628-pages-articles.xml.bz2"

if [ -n "$1" ]
then
  arguments=$1
fi
java -cp $cp:$CLIX_PATH/bin de.mpii.clix.wikipedia.Wiki2XML $arguments
