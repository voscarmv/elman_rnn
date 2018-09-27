#!/bin/bash

CTR=1

while test $CTR -le 31 ; do
	./musigma2.sh $2 $1 $CTR
	CTR=`expr $CTR + 1`
done
