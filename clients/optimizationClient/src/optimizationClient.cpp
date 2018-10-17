//============================================================================
// Name        : optimizationClient.cpp
// Author      : F.Péan
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "ProblemManager.h"
#include "helper.h"

using namespace optclient;

int main(int argc, char *argv[]) {

//	helper::test(argv[2]);
	ProblemManager problem(argc, argv);
	problem.initialize();
	problem.setup();
	problem.solve();
	problem.finalize();

	return 0;
}
