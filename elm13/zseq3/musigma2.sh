#!/bin/bash

# Example
#	$ ./check31.sh w_1_g_M5.out

SXS=`cat $1 | grep "\.$2\." | sed 's/.*sxs //;s/, .*//' | grep '1' | wc -l`
ACT=`cat $1| grep "\.$2\." | sed 's/.*act //;s/, .*//' | grep -- "^-1$" | wc -l`
PRED=`cat $1 | grep "\.$2\." | sed 's/.*pred //;s/, .*//' | grep -- "^-1$" | wc -l`
TOT=`cat $1 | grep "\.$2\." | wc -l`

# P1=`echo "$ACT * 100 / $TOT" | bc`
# P2=`echo "$PRED * 100 / $TOT" | bc`

P1x=`echo "scale=4;$ACT / $TOT" | bc`
P2x=`echo "scale=4;$PRED / $TOT" | bc`

# echo "p1 = $P1x, p2 = $P2x, sxs = $SXS"

Q=`echo "scale=4;($P1x * $P2x) + ((1 - $P1x) * (1 - $P2x))" | bc`

MIU=`echo "scale=4;$TOT * $Q" | bc`

SIG=`echo "scale=4;sqrt($TOT * $Q * (1 - $Q))" | bc`

#echo Q $Q
#echo MIU $MIU
#echo SIG $SIG

Z=`echo "scale=4;($SXS - $MIU) / $SIG" | bc`

#PERC=`echo "scale=4; $SXS / $TOT" | bc`

#./prob_3 $TOT 10000 $P1 $P2

# THEOMU=`./prob_3 $TOT 10000 $P1 $P2 | sed 's/.*mu //;s/,.*//'`
# DIFFMU=`echo "$SXS - $THEOMU" | bc`

# echo perc $PERC
# echo "diffmu $DIFFMU"
echo $Z
