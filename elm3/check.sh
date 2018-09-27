#!/bin/bash

while : ; do
	FILE=`ls epoch*.dat | sort -V | tail -n 1`
	echo 'set term wxt noraise'
	echo 'set xrange [0:100]'
	echo 'set yrange [0:1]'
	echo 'set key off'
	CTR=15
	printf 'plot '
	while test $CTR -ge 3 ; do
		COLOR=`echo "(255*$CTR)/16" | bc`
		printf '"%s" using %d with lines lc rgb "#%02x%02x%02x", ' "$FILE" "$CTR" "$COLOR"  "$COLOR"  "$COLOR" 
		CTR=`expr $CTR - 1`
	done
	printf '"%s" using %d with lines lc rgb "#%02x%02x%02x", ' "$FILE" 16 "255"  "255"  "255"
	printf '"%s" using 1 with lines lc rgb "#FF0000", "%s" using 2 with lines lc rgb "#00FF00"\n' "$FILE" "$FILE"
	sleep 1
done | gnuplot
