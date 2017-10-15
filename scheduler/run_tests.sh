#!/bin/bash
for run in {1..7}; do
    ./a.out inputs/input-$run.txt >> my-outputs/hprn-my-outputs-$run.txt;
done
