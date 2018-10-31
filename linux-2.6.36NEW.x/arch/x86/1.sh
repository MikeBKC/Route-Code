#!/bin/bash
file=`find ./ -name "*"`
echo "$file"
for i in $file
do
    if [ "$i" != "1.sh" ];then
    cvs add $i
    cvs ci -m "7620" 
    fi
done
