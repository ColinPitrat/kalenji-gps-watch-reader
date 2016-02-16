#!/bin/sh

function syntaxe()
{
	echo "Syntaxe: $0 <version>"
}

if [ -z "$1" ]
then
	syntaxe
	exit 1
fi
if [ ! -z "$2" ]
then
	syntaxe
	exit 2
fi

VERSION=$1
DELIVERY_FILE="kalenji-gps-watch-reader-${VERSION}.tar.gz" 
WIN_DELIVERY_FILE="kalenji-gps-watch-reader-${VERSION}_win.zip" 
if [ -f $DELIVERY_FILE ]
then
	echo "$DELIVERY_FILE already exists. Suppress it if you really want to overwrite it."
	exit 3
fi
if [ -f $WIN_DELIVERY_FILE ]
then
	echo "$WIN_DELIVERY_FILE already exists. Suppress it if you really want to overwrite it."
	exit 3
fi

rm -rf kalenji-gps-watch-reader
mkdir kalenji-gps-watch-reader
cp -r ../etc/ ../src/ ../Makefile ../scripts/ ../README.md kalenji-gps-watch-reader
pushd kalenji-gps-watch-reader
make clean
popd
tar -cvzf $DELIVERY_FILE kalenji-gps-watch-reader

pushd kalenji-gps-watch-reader
make windows
cp ../zadig.exe win/
cp -r win kalenji-gps-watch-reader
zip -r $WIN_DELIVERY_FILE kalenji-gps-watch-reader
popd
mv kalenji-gps-watch-reader/$WIN_DELIVERY_FILE .

echo "$DELIVERY_FILE and $WIN_DELIVERY_FILE are ready"
