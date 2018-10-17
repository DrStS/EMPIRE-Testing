#!/bin/bash
if [ $# -eq 1 ]; then
	if [ $1 = 'ICC' ]; then
		Emperor emperorInput.xml & EPID=$!
		sleep 1s

		blockMesh -case turekFSIBenchmarkOF
		decomposePar -force -case turekFSIBenchmarkOF
		mpirun -np 4 pimpleDyMFsiFoam -parallel -case turekFSIBenchmarkOF >/dev/null 2>outputPimpleDyMFsiFoam.err &
		carat turekFSIBenchmarkCarat/turekFSIBenchmarkCarat.dat >/dev/null 2>outputCarat.err &

		wait $EPID
	
	elif [ $1  = 'GCC' ]; then
		echo "Stop openmp threading"
	
	else
		echo   'Please input ICC or GCC.'
	fi
else
	echo   'Please input ICC or GCC.'
fi