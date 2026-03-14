#!/bin/bash

echo "输入两个字符串"
read n1
read n2

if test $n1 \> $n2
then
    echo "n1 > n2"
elif test $n1 \< $n2
then
    echo "n1 < n2"
else
    echo "$n1 = $n2"

fi