#!/bin/sh
(
    for i in $@
    do
      echo -ne "\x$i"
    done 
    echo "" 
) | hexdump -C
