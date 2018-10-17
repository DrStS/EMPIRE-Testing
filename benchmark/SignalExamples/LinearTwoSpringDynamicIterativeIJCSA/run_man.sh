#!/bin/bash
xterm  -hold -geometry 140x40 -e mpirun -np 1 Emperor emperorInput.xml &
sleep 0.4s
xterm  -hold -e mpirun -np 1 python subSystem1/subSystem1.py &
sleep 0.4s 
xterm  -hold -e mpirun -np 1 python subSystem2/subSystem2.py &