MIN="0.13"
MAX="0.67"
LOW="0.5"
HIGH="0.51"
FACTOR=`echo "scale = 4; ($HIGH - $LOW) / ($MAX - $MIN)" | bc`
while read LINE ; do
	echo "scale = 4; (($LINE - $MIN) * $FACTOR) + $LOW" | bc
done
