#!/bin/bash

while : ; do
	FILE=`ls epoch*.dat | sort -V | tail -n 1`
	echo 'set term wxt noraise'
	echo 'set xrange [0:100]'
	echo 'set yrange [-.7:.7]'
	echo 'set key off'
	CTR=15
	printf 'plot '
	while test $CTR -ge 3 ; do
		COLOR=`echo "(255*$CTR)/16" | bc`
		printf '"%s" using %d with lines lc rgb "#%02x%02x%02x", ' "$FILE" "$CTR" "0"  "0"  "0" 
		CTR=`expr $CTR - 1`
	done
	printf '"%s" using %d with lines lc rgb "#%02x%02x%02x", ' "$FILE" 16 "0"  "0"  "0"
	printf '"%s" using 1 with lines lc rgb "#FF0000" lw 2, "%s" using 2 with lines lc rgb "#22FF22" lw 2, "ts.dat" using 2 with lines lc rgb "#0000FF" lw 2\n' "$FILE" "$FILE"
	sleep 1
done | gnuplot
