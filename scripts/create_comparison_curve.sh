#!/bin/sh

sed '1d' $1 > /tmp/comparison_full.csv
shift

HEADER="Time,Distance"
REPLACE_COMMA=",,"
while [ "$1" != "" ]
do
	sed '1d' $1 > /tmp/to_compare.csv
	join --nocheck-order -j 1 -t , -a 1 /tmp/comparison_full.csv /tmp/to_compare.csv > /tmp/comparison.csv
	join --nocheck-order -j 1 -t , -v 2 /tmp/comparison_full.csv /tmp/to_compare.csv > /tmp/comparison_missing.csv
	sed -i "s/,/$REPLACE_COMMA/" /tmp/comparison_missing.csv
	cat /tmp/comparison.csv /tmp/comparison_missing.csv | sort -n > /tmp/comparison_full.csv
	REPLACE_COMMA="${REPLACE_COMMA},"
	HEADER="${HEADER},Distance"
	shift
done

(echo "$HEADER" && cat /tmp/comparison_full.csv) > comparison.csv
echo "Output available in comparison.csv"
