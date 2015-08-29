#!/bin/sh
for file in *lnk 
do 
    wget -O `echo $file | sed 's/lnk/png/'` `cat $file` 
done
