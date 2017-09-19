#!/bin/bash
g++ -std=c++11 linker.cpp
for run in {1..8}; do
    echo Output for Input $run:;
    ./a.out inputs-outputs/input-$run;
    echo;
done
