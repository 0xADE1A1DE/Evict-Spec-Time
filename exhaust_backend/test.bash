#!/bin/bash

file=$2
mv result.csv result_prev.csv
gcc main.c -DNOPDEF=$1 -o crun -lmastik -lassemblyline

taskset -c 1 ./crun > result.csv
cp result.csv $file.csv
python3 plot.py $file
