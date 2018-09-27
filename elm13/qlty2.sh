#!/bin/bash

FILE=$1
CTR=0

while test $CTR -le 200 ; do
	SUCCESS=`cat $FILE | grep "qlty $CTR$" | grep "sxs 1" | wc -l`
	FAILURE=`cat $FILE | grep "qlty $CTR$" | grep "sxs 0" | wc -l`
	SF=`expr $SUCCESS + $FAILURE`
#	echo -n "$SUCCESS / ($SUCCESS + $FAILURE) = "
	echo -n "$CTR "
	if test $SF -eq 0 ; then
		echo "?"
	else
		echo "scale=5;$SUCCESS / ($SUCCESS + $FAILURE)" | bc
	fi
	CTR=`expr $CTR + 2`
done
