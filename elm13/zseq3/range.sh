
CTR=1

while test $CTR -le 1600 ; do
	./check31b.sh zseq$CTR.dat | awk '{ total += $1; count++ } END { print total/count }'
	CTR=`expr $CTR + 1`
done
