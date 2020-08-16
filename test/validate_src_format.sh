#!/bin/sh

rc=0

echo " - Verify that no source file is space indented"
grep -rlI "^ " src/
if [ $? -eq 0 ]
then
  echo "ERROR: Found some file in src/ that are indented with spaces (see which ones above)"
	rc=1
fi

echo " - Verify that no source file has line ending with space"
grep -rlI " $" src/
if [ $? -eq 0 ]
then
  echo "ERROR: Found some file in src/ that have lines ending with spaces (see which ones above)"
	rc=1
fi

exit $rc
