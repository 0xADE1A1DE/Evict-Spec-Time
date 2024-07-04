#!/bin/bash

rm -f round1_benchmark_result.csv

for i in {1..10}
do
  ./crun $i
  python3 ./scripts/benchmark_round1.py
  tail -1 round1_benchmark_result.csv
  echo $i
done

python3 ./scripts/plot_round1_accuracy.py