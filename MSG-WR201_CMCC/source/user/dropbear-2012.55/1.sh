#!/bin/bash
file=`find ./ -name "*"`
echo "$file"
for i in $file
do
    if [ "$i" != "1.sh" ];then
    cvs add $i
    cvs ci -m "dropbear-2012.55" 
    fi
done
