#!/bin/bash

CTR=1

while test $CTR -le 31 ; do
	./musigma.sh $1 $CTR
	CTR=`expr $CTR + 1`
done
