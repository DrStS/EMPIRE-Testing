#!/bin/bash
xterm -hold -e mpirun -np 1 Emperor emperorInput.xml & EPID=$!
sleep 0.8s
xterm -hold -e mpirun -np 1 python ping.py &
sleep 0.8s 
xterm -hold -e mpirun -np 1 python pong.py &
wait $EPID
