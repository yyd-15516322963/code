#!/bin/bash

sum=0
for i in `seq 1 100`
do
    sum=`expr $i + $sum`
done

echo "1-100之和为：$sum"