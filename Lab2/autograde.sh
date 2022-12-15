#!/bin/bash
​
### Build your executable
make
​
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NOCOLOR='\033[0m'
​
SCORE=0
​
### Autograder for counter
​
EXEC="./counter"
THREADS="2"
chmod u+x $EXEC
​
​
TESTDIR=""
if [ $# -eq 1 ] 
then
    TESTDIR=$1
else
    TESTDIR="./autograde_tests"
fi
​
​
for prim in "--lock=tas" "--lock=ttas" "--lock=ticket" "--lock=pthread" "--bar=sense"; do
	for file in $TESTDIR/*; do
		if [ "${file: -4}" == ".cnt" ]
		then
			IN=$file
			CASE=${IN%.*}
			MY=$CASE.my
			read -r ITERS < $IN
			ANS=$((ITERS*THREADS))
			$EXEC -o $MY -t $THREADS -i $ITERS $prim
			read -r MYANS < $MY
			if [ $MYANS -eq $ANS ]
			then 
				SCORE=$(($SCORE+2))
				echo -e $ITERS $prim "..... ${GREEN}Pass${NOCOLOR}"
			else 
				echo -e $ITERS $prim "..... ${RED}FAIL${NOCOLOR}"
			fi
		fi
	done
done
​
​
#### Autograder for sort
​
### Generate a test file
### (of unspecified range and size)
# shuf -i0-2147483643 -n382 > case1.txt
​
### Sort it using sort
# sort -n case1.txt > soln1.txt
​
EXEC="./mysort"
THREADS="2"
chmod u+x $EXEC
​
​
​
for prim in "--lock=tas" "--lock=ttas" "--lock=ticket" "--lock=pthread" "--bar=sense"; do
	for file in $TESTDIR/*; do
		if [ "${file: -4}" == ".txt" ]
		then
			IN=$file
			CASE=${IN%.*}
			MY=$CASE.my
			ANS=$CASE.ans
			$EXEC $IN -o $MY -t $THREADS $prim
			if cmp --silent $MY $ANS;
			then 
				SCORE=$(($SCORE+8))
				echo -e $CASE $prim "..... ${GREEN}Pass${NOCOLOR}"
			else 
				echo -e $CASE $prim "..... ${RED}FAIL${NOCOLOR}"
			fi
		fi
	done
done
SCORE=$(echo "scale=1; $SCORE/2" | bc -l)
​
echo -e "${YELLOW}Score:" $SCORE "${NOCOLOR}"
​
RET=1
if [ $SCORE == "25.0" ]
	then
	RET=0
fi
​
exit $RET
Collapse

