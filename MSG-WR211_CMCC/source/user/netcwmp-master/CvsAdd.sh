#!/bin/bash
file=`cat list`
for i in $file
do
    if [ "$i" != "./CvsAdd.sh" ];then
    echo "cvs adding " $i
    cvs add $i
    #echo "cvs cing -m \"TR069 �ͻ���\" " $i
    cvs ci -m "TR069 �ͻ���"
    fi
done
