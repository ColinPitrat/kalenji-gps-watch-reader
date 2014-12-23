#!/bin/sh

export TZ="Europe/Paris"

RESULT_FILE=`pwd`/test_result_`date +%Y-%m-%d_%H-%M-%S`.log 
OK_CASES=0
KO_CASES=0
diff_command=""
echo "" > $RESULT_FILE
for case in case*
do
	echo " ===== Case $case =====" >> $RESULT_FILE
	pushd $case > /dev/null
	./run.sh >> $RESULT_FILE
	pushd results > /dev/null
	for result in *
	do
		diff $result /tmp/$result >> $RESULT_FILE
		if [ $? -eq 0 ]
		then
			echo "OK - $case - $result" | tee -a $RESULT_FILE
			let OK_CASES=$OK_CASES+1
		else
			echo "KO - $case - $result" | tee -a $RESULT_FILE
			diff_command="$diff_command
vi -d `pwd`/$result /tmp/$result"
			let KO_CASES=$KO_CASES+1
		fi
	done
	popd > /dev/null
	popd > /dev/null
done

percentage=$((100*$OK_CASES/($OK_CASES+$KO_CASES)))
echo ""
echo "Result = $percentage % ($OK_CASES OK - $KO_CASES KO)"
echo "See $RESULT_FILE for details"
echo ""
echo "To obtain diff:"
echo "$diff_command"
