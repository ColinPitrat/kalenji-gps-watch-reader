#!/bin/bash

export TZ="Europe/Paris"

RESULT_FILE=`pwd`/test_result_`date +%Y-%m-%d_%H-%M-%S`.log 
OK_CASES=0
KO_CASES=0
diff_command=""
echo "" > $RESULT_FILE
for c in cas*
do
	echo " ===== Case $c =====" >> $RESULT_FILE
	pushd $c > /dev/null
	./run.sh >> $RESULT_FILE
	pushd results > /dev/null
	for result in *
	do
		diff $result /tmp/$result >> $RESULT_FILE
		if [ $? -eq 0 ]
		then
			echo "OK - $c - $result" | tee -a $RESULT_FILE
			let OK_CASES=$OK_CASES+1
		else
			echo "KO - $c - $result" | tee -a $RESULT_FILE
			diff_command="$diff_command
vi -d `pwd`/$result /tmp/$result"
			let KO_CASES=$KO_CASES+1
            echo "travis_fold:start:test_error$KO_CASES"
            diff -u `pwd`/$result /tmp/$result
            echo "travis_fold:end:test_error$KO_CASES"
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
if [ ! -z "$diff_command" ]
then
    echo "To obtain diff:"
    echo "$diff_command"
    echo ""
    exit 1
fi
