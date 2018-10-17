#!/bin/bash
xterm -hold -geometry 140x40 -e mpirun -np 1 Emperor emperorInput.xml & EPID=$!
sleep 0.8s
xterm -hold -e mpirun -np 1 python controlUnit.py   &
sleep 0.8s 
xterm -hold -e mpirun -np 1 python generatorUnit.py &
sleep 0.8s 
xterm -hold -e mpirun -np 1 python dummyOF.py       &
wait $EPID
