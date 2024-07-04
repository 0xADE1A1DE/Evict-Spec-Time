#!/bin/bash

## Loop from 1 to 100 with step 5
for i in {0..150..5}
do
    ### Change line 111 and 113 in first_round.c to asm volatile (".rept $i;\nNOP;\n.endr");
    sed -i "111s/.*/asm volatile (\".rept $i;\\\nNOP;\\\n.endr\");/" first_round.c
    sed -i "113s/.*/asm volatile (\".rept $i;\\\nNOP;\\\n.endr\");/" first_round.c

    make
    for j in {1..5}
    do
        ./crun $j
        python3 ./scripts/benchmark_round1.py
        tail -1 round1_benchmark_result.csv
    done
    echo $i
done