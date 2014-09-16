#!/bin/sh

if [ -z "$1" ]
then
	echo "Usage: $0 <GPX file>"
	echo " GPX File: GPX file to export to the watch"
fi
FILE=`realpath "$1"`
BINPATH=`realpath "$0"`
BINDIR=`dirname "$BINPATH"`
BINDIR=`dirname $(realpath "$BINDIR")`

pushd $BINDIR > /dev/null
echo $BINDIR
echo $BINPATH
./kalenji_reader -D GPX -i "$FILE" -f FixElevation,ReducePoints,ComputeSessionStats -o Kalenji
popd > /dev/null
