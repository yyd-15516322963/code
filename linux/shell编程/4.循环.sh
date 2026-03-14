#!/bin/bash

# for i in `seq 1 10`
# do
#     echo "hello world"

# done

i=0
while test $i -lt 10
do
    echo "hello"
    i=`expr $i + 1`
done