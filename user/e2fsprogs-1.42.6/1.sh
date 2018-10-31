#!/bin/bash
file=`find ./ -name "*"`
echo "$file"
for i in $file
do
    if [ "$i" != "1.sh" ];then
    cvs add $i
    fi
done
