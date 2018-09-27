#!/bin/bash

CTR=0
while test $CTR -lt 31 ; do
	echo Time $CTR
	cat dsy.dat | ./elmanx | tee "outx$CTR.dat"
	CTR=`expr $CTR + 1`
done
