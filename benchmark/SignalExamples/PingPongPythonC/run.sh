#!/bin/bash
mpirun -np 1 Emperor emperorInput.xml > Emperor.log 2> Emperor.err & EPID=$!
sleep 0.4s
mpirun -np 1 ping/binICC/ping > ping.log 2>ping.err &
sleep 0.4s 
mpirun -np 1 python pong.py > pong.log 2>pong.err &
wait $EPID