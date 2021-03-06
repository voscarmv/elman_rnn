#!/bin/bash

FILE=$1
CTR=0

while test $CTR -le 120 ; do
	SUCCESS=`cat $FILE | grep "qlty $CTR$" | grep "sxs 1" | wc -l`
	FAILURE=`cat $FILE | grep "qlty $CTR$" | grep "sxs 0" | wc -l`
	echo -n "$SUCCESS / ($SUCCESS + $FAILURE) = "
	echo "scale=5;$SUCCESS / ($SUCCESS + $FAILURE)" | bc
	CTR=`expr $CTR + 2`
done
