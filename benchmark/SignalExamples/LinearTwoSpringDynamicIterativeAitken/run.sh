#!/bin/bash
mpirun -np 1 Emperor emperorInput.xml > Emperor.log 2> Emperor.err & EPID=$!
sleep 0.4s
mpirun -np 1 python subSystem1/subSystem1.py > subSystem1.log 2>subSystem1.err &
sleep 0.4s 
mpirun -np 1 python subSystem2/subSystem2.py > subSystem2.log 2>subSystem2.err &
wait $EPID