#!/bin/bash

# init
function pause(){
   read -p “$*”
}


if ! [ -L "./index.aspt" ]; then
  ln -s /users/ant/index/index.aspt .
fi

if ! [ -L "./doclist.aspt" ]; then
  ln -s /users/ant/index/doclist.aspt .
fi

AFFILIATION=`grep "^ *affiliation.*=" ltw.conf | cut -f 2 -d =`
AFFILIATION=${AFFILIATION/ /}
echo "affiliation: $AFFILIATION"
TASK=`grep "^ *task.*=" ltw.conf | cut -f 2 -d =`
TASK=${TASK/ /}
echo "task: $TASK"
NAME=`grep "^ *name.*=" ltw.conf | cut -f 2 -d =`
NAME=${NAME/ /}
echo "name: $NAME"
ID=`grep "^ *id.*=" ltw.conf | cut -f 2 -d =`
ID=${ID/ /}
ID=`echo -n $ID`

OUT=${AFFILIATION}_${TASK}_${NAME}_${ID}.xml

if [ -z $OUT ]; then
  "No output file specified"
  exit
fi

echo "Output to: \"$OUT\""

if [ -f $OUT ]; then
  pause "The run file is already existing, are you sure to override it? Press Ctrl-C to stop running me"
fi

ltw -lowercase -targets:1 -anchors:250 -index /home/monfee/corpus/inex/2009/ltw/index/links.idx /home/monfee/corpus/inex/2009/ltw/2009_33Topics/*.xml > $OUT
