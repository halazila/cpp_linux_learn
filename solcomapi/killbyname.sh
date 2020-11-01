#!/bin/bash
line=`ps -a | grep $1`
arr=($line)
for((i=0;i<${#arr[*]};i=i+4))
{
    if [ -d /proc/${arr[i]} ]; then
        kill -9 ${arr[i]}
    fi
}