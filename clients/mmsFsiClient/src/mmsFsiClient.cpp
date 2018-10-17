// set by rupert 2013/06/21

#include "EMPIRE_API.h"
#include <assert.h>
#include "GiDFileIO.h"
#include "FluxCreator.h"
#include "FieldCreator.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <stdlib.h>

using namespace std;

void dummyUpdataPDE() {
	// t = t + delta_t;
	// set up the PDE at this time step
}
void dummySolvePDE() {
	// solve the PDE
}
void mmsDeviation(double *recv, double *target, double *deviation,
		int numNodes) {
	//calculate the deviation between the calculated field and the target field
	for (int j = 0; j < numNodes * 3; j++) {
		deviation[j] = recv[j] - target[j];
	}
}

bool doIterativeCoupling(string text) {
	if (text == "iterativeCoupling")
		return true;
	else if (text == "looseCoupling")
		return false;
	else
		assert(false);
	return false;
}

bool getBool(string text) {
	if (text == "true" || text == "yes")
		return true;
	else if (text == "false" || text == "no")
		return false;
	else
		cout << "boolian input string is not valid: use true/false or yes/no"
				<< endl;
	exit(-1);

}

int getInt(string text) {
	stringstream ss(text);
	int numTimeSteps;
	ss >> numTimeSteps;
	return numTimeSteps;
}
double getDouble(string text) {
	stringstream ss(text);
	double myDbl;
	ss >> myDbl;
	return myDbl;
}
/*
 * The mmsClient is coupled with carat.
 * The client  reads the gid mesh, sends force data field to the server (resp. carat) and evaluates the displacement error on an additional probe mesh
 */
int main(int argc, char **argv) {
	if (argc != 2) {
		cerr << "Error: Please provide a valid input file." << endl;
		exit (EXIT_FAILURE);
	}
	EMPIRE_API_Connect(argv[1]);

	bool todoIterativeCoupling = doIterativeCoupling(
			EMPIRE_API_getUserDefinedText("couplingType"));
	string meshfile = EMPIRE_API_getUserDefinedText("GiDMeshFile");
	string probefile = EMPIRE_API_getUserDefinedText("GiDProbeFile");
	string mmsFunction = EMPIRE_API_getUserDefinedText("mmsFunction");
	int numTimeSteps = getInt(EMPIRE_API_getUserDefinedText("numTimeSteps"));
	double timeStep = getDouble(EMPIRE_API_getUserDefinedText("timeStep"));
	double loadIncrement = getDouble(EMPIRE_API_getUserDefinedText("loadIncrement"));
	int nTimeProbes = getInt(EMPIRE_API_getUserDefinedText("mmsNTimeProbes"));
	int probeFrequency=numTimeSteps/nTimeProbes;
	cout << "mmsFsiClient: probe output is performed all "<< probeFrequency << " timesteps!"<< endl;

	stringstream sourceF;
	sourceF << mmsFunction << "SourceF";
	stringstream targetD;
	targetD << mmsFunction << "TargetD";
	stringstream targetF;
	targetF << mmsFunction << "TargetF";
	string mmsSourceF = sourceF.str();
	string mmsTargetD = targetD.str();
	string mmsTargetF = targetF.str();

	cout << "TARGET-F STRING is called "<< mmsTargetF<< endl;

	int numNodes = -1;
	int numProbes = -1;
	int numElems = -1;
	int nodesPerElem = -1;
	int nodesPerElemP = -1;
	double loadFac = 0.0;
	double currentTime = 0.0;
	int numCouplingIter = 0;

	double *nodeCoors;
	int *nodeIDs;
	int *numNodesPerElem;
	int *elemTable;
	int *elemIDs;

	double *nodeCoorsP;
	int *nodeIDsP;
	int *numNodesPerElemP;
	int *elemTableP;
	int *elemIDsP;

	//std::cout<< "reading in GiD mesh file" << std::cout;
	//sending probe file
	GiDFileIO::readDotMsh(probefile, numProbes, numElems, nodeCoorsP, nodeIDsP,
			numNodesPerElemP, elemTableP, elemIDsP);
	EMPIRE_API_sendMesh("defaultField",numProbes, numElems, nodeCoorsP, nodeIDsP,
			numNodesPerElemP, elemTableP);
	//cout << "number of probes: " << numProbes << endl;

	//sending mesh
	cout << "mmsFsiClient: read mesh:" << endl;
	GiDFileIO::readDotMsh(meshfile, numNodes, numElems, nodeCoors, nodeIDs,
			numNodesPerElem, elemTable, elemIDs);
	cout << "mmsFsiClient: send mesh:" << endl;
	EMPIRE_API_sendMesh("defaultField",numNodes, numElems, nodeCoors, nodeIDs, numNodesPerElem,
			elemTable);

	cout << "mmsFsiClient: finalizing mesh:" << endl;
	nodesPerElem = numNodesPerElem[0];
	nodesPerElemP = numNodesPerElemP[0];

	//Ftilde is the additional MMS source force
	double *dofsFtildeTotal = new double[numNodes * 3];
	double *dofsFtilde = new double[numNodes * 3];

	//F is the force received from openFOAM
	double *dofsFTotal = new double[numNodes * 3];
	double *dofsF = new double[numNodes * 3];

	//Fmerge is the total force of F and Ftilde
	double *dofsFmergeTotal = new double[numNodes * 3];
	double *dofsFmerge = new double[numNodes * 3];

	//Fhat is the MMS target force of F
	double *dofsFhatTotal = new double[numNodes * 3];
	double *dofsFhat = new double[numNodes * 3];

	//double *dofsDiscreteForces = new double[numNodes * 3];

	//D is the displacement received from carat
	double *dofsDTotal = new double[numNodes * 3];
	double *dofsD = new double[numNodes * 3];

	//Dhat is the target displacement of D
	double *dofsDhatTotal = new double[numNodes * 3];
	double *dofsDhat = new double[numNodes * 3];

	//DEVIATIONS OF THE FIELDS
	double *dofsFDev = new double[numNodes * 3];
	double *dofsDDev = new double[numNodes * 3];
	double *dofsEDev = new double[numNodes * 3]; //Energy Deviation

	//PROBE Fields
	/*double *probesF = new double[numProbes * 3];
	 double *probesD = new double[numProbes * 3];
	 double *probesE = new double[numProbes * 3];
	 */
	double *probesFDev = new double[numProbes * 3];
	double *probesDDev = new double[numProbes * 3];
	double *probesEDev = new double[numProbes * 3];

	double *currentPos = new double[numNodes * 3];;



	for (int j = 0; j < numNodes * 3; j++) {
		dofsFtildeTotal[j] 	= 0.0;
		dofsFtilde[j] 		= 0.0;
		dofsFTotal[j] 		= 0.0;
		dofsF[j] 			= 0.0;
		dofsFmergeTotal[j] 	= 0.0;
		dofsFmerge[j] 		= 0.0;
		dofsFhatTotal[j]	= 0.0;
		dofsFhat[j] 		= 0.0;
		dofsDTotal[j] 		= 0.0;
		dofsD[j] 			= 0.0;
		dofsDhatTotal[j] 	= 0.0;
		dofsDhat[j] 		= 0.0;
		dofsFDev[j] 		= 0.0;
		dofsDDev[j] 		= 0.0;
		dofsEDev[j] 		= 0.0;
		currentPos[j]		= 0.0;


	}
	for (int j = 0; j < numProbes * 3; j++) {
		probesFDev[j] 		= 0.0;
		probesDDev[j]		= 0.0;
		probesEDev[j] 		= 0.0;
	}


	cout << "mmsFsiClient: the following mms Function is used: " << mmsFunction << endl;

	FieldCreator *fcFtilde = new FieldCreator(numNodes, numElems, nodesPerElem, nodeCoors, nodeIDs, elemTable, mmsSourceF);
	//FieldCreator *fcF = new FieldCreator(numNodes, numElems, nodesPerElem, nodeCoors, nodeIDs, elemTable, mmsSource);

	FieldCreator *fcFhat = new FieldCreator(numNodes, numElems, nodesPerElem, nodeCoors, nodeIDs, elemTable, mmsTargetF);
	FieldCreator *fcDhat = new FieldCreator(numNodes, numElems, nodesPerElem, nodeCoors, nodeIDs, elemTable, mmsTargetD);


	cout << "mmsFsiClient: Number of " << nodesPerElem << "-node Elements: " << numElems << endl;
	cout << "mmsFsiClient: Number of Nodes: " << numNodes << endl;
	cout << "mmsFsiClient: Number of Probes: " << numProbes << endl;

	string dataFileName(meshfile);
	dataFileName.erase(dataFileName.size() - 4, 4);
	dataFileName.append(".res");

	string probeFileName(probefile);
	probeFileName.erase(probeFileName.size() - 4, 4);
	probeFileName.append(".res");

	GiDFileIO::initDotRes(dataFileName);


	ofstream probesF("probesF");
	ofstream probesD("probesD");
	ofstream probesE("probesE");

	probesF << "Timestep   ";
	for (int j = 1; j <= numProbes; j++) {
		probesF << j << "-fx \t \t \t " << j << "-fy \t \t \t " << j
				<< "-fz \t \t \t ";
		probesD << j << "-dx \t \t \t " << j << "-dy \t \t \t " << j
				<< "-dz \t \t \t ";
		probesE << j << "-ex \t \t \t " << j << "-ey \t \t \t " << j
				<< "-ez \t \t \t ";

	}
	probesF << endl;
	probesD << endl;
	probesE << endl;


	bool steadyState = 0;
	if (loadIncrement != -1.0) {
		cout << "it's a steady simulation" << endl;
		steadyState = 1;
		fcFtilde->create(dofsFtildeTotal);
		cout << "creating mms force field finished" << endl;
		fcFhat->create(dofsFhatTotal);
		cout << "creating force target field finished" << endl;
		fcDhat->create(dofsDhatTotal);
		cout << "creating displacement target field finished" << endl;
	}


	for (int j = 0; j < numNodes * 3; j++) {
		currentPos[j] = nodeCoors[j];
	}


	//TIMELOOP
	cout << "Starting time loop with a time step size of " << timeStep << endl;
	for (int i = 1; i <= numTimeSteps; i++) {

		currentTime = timeStep * double(i);
		cout << "Current Time is " << currentTime << endl;

		if (!steadyState) {
			//cout << "UNSTEADY STATE SOURCE CALCULATION" << endl;
			fcFtilde->create(dofsFtilde, currentTime);
			fcFhat->create(dofsFhat,currentTime);
			fcDhat->create(dofsDhat, currentTime);
		}
		else { //if (steadyState){
			if (loadFac < 1.0)
				loadFac += loadIncrement;
			else
				loadFac = 1.0;
			cout << "current load factor is " << loadFac << endl;

			if (loadFac < 1.0) {
				for (int j = 0; j < numNodes * 3; j++) {
					dofsFtilde[j] = dofsFtildeTotal[j] * loadFac;
				}
			} else {
				for (int j = 0; j < numNodes * 3; j++) {
					dofsFtilde[j] = dofsFtildeTotal[j];
					dofsFhat[j]	  = dofsFhatTotal[j];
					dofsDhat[j]   = dofsDhatTotal[j];
				}
			}
		}


		//COUPLING
		if (!todoIterativeCoupling) {
			//LOOSE COUPLING

			//FLUID
				EMPIRE_API_sendDataField("defaultField",numNodes * 3, dofsD);//for full FSI it should be dofsD
				EMPIRE_API_recvDataField("defaultField",numNodes * 3, dofsF);

			//Sum the received fluid forces and the additional MMS forces
			for (int j = 0; j < numNodes*3; j++) {
				dofsFmerge[j] =  dofsFtilde[j] + dofsF[j];
			}

			//STRUCTURE
			//EMPIRE_API_sendDataField("defaultField",numNodes * 3, dofsFtilde);//dofsFtilde is currently the total needed force for the structure in the current state
			EMPIRE_API_sendDataField("defaultField",numNodes * 3, dofsFmerge);//dofsFtilde is currently the total needed force for the structure in the current state
			EMPIRE_API_recvDataField("defaultField",numNodes * 3, dofsD);

			//

		} else {
			//ITERATIVE COUPLING

			cout << "mmsFsiClient: entering iterative coupling loop" << endl;
			do {

				//FLUID
				EMPIRE_API_sendDataField("defaultField",numNodes * 3, dofsD);
				EMPIRE_API_recvDataField("defaultField",numNodes * 3, dofsF);

				//Sum the received fluid forces and the additional MMS forces
				for (int j = 0; j < numNodes * 3; j++) {
					dofsFmerge[j] =  dofsFtilde[j] + dofsF[j];
				}

				//STRUCTURE
				//EMPIRE_API_sendDataField("defaultField",numNodes * 3, dofsFtilde);//dofsFtilde is currently the total needed force for the structure in the current state
				EMPIRE_API_sendDataField("defaultField",numNodes * 3, dofsFmerge);//dofsFtilde is currently the total needed force for the structure in the current state

				EMPIRE_API_recvDataField("defaultField",numNodes * 3, dofsD);//has to be later dofsD

				cout << "checking FSI convergence" << endl;


			} while (EMPIRE_API_recvConvergenceSignal() == 0);
		}


		//CALCULATE FIELD DEVIATIONS FOR F and D
		if (loadFac == 1.0 || steadyState == 0) {
			mmsDeviation(dofsF, dofsFhat, dofsFDev, numNodes);
			mmsDeviation(dofsD, dofsDhat, dofsDDev, numNodes);
		}


		//Calculate the energy deviation of the field
		for (int j = 0; j < numNodes * 3; j++) {
			dofsEDev[j] = dofsFDev[j] * dofsDDev[j];
		}


		//MAPPING TO THE PROBES

		//map displacement deviations on the probe mesh
		cout << "map the displacements to the probes" << endl;
		EMPIRE_API_sendDataField("defaultField",numNodes * 3, dofsDDev);
		EMPIRE_API_recvDataField("defaultField",numProbes * 3, probesDDev);

		//map force deviations on the probe mesh
		cout << "map the forces to the probes"<< endl;
		EMPIRE_API_sendDataField("defaultField",numNodes * 3, dofsFDev);
		EMPIRE_API_recvDataField("defaultField",numProbes * 3, probesFDev);


		//WRITE PROBES
		if (loadFac == 1.0 || steadyState == 0) {
			if(i % probeFrequency==0){
			probesF << i << " \t       ";
			probesD << i << " \t       ";
			probesE << i << " \t       ";
			for (int j = 0; j <= 3 * numProbes; j++) {

				probesF << probesFDev[j] << " \t ";
				probesD << probesDDev[j] << " \t ";
				//probesE << probesFDev[j]*probesDDev[j] << " \t ";
			}
			probesF << endl;
			probesD << endl;
			probesE << endl;
		}}



	/*	GiDFileIO::appendNodalDataToDotRes(dataFileName,
				"\"mmsDiscreteForces\"", "\"mms carat testing\"", i, "Vector",
				numNodes, nodeIDs, dofsDiscreteForces);


		GiDFileIO::appendNodalDataToDotRes(dataFileName, "\"mmsF\"",
				"\"mms carat testing\"", i, "Vector", numNodes, nodeIDs,
				dofsF);
		GiDFileIO::appendNodalDataToDotRes(dataFileName, "\"mmsFhat\"",
				"\"mms carat testing\"", i, "Vector", numNodes, nodeIDs,
				dofsFhat);*/


		GiDFileIO::appendNodalDataToDotRes(probeFileName, "\"probesFDev\"",
				"\"mms carat testing\"", i, "Vector", numProbes, nodeIDsP,
				probesFDev);


		GiDFileIO::appendNodalDataToDotRes(dataFileName, "\"mmsFResults\"",
				"\"mms carat testing\"", i, "Vector", numNodes, nodeIDs,
				dofsF);
		GiDFileIO::appendNodalDataToDotRes(dataFileName, "\"mmsDResults\"",
				"\"mms carat testing\"", i, "Vector", numNodes, nodeIDs,
				dofsD);


		GiDFileIO::appendNodalDataToDotRes(dataFileName, "\"mmsFTarget\"",
				"\"mms carat testing\"", i, "Vector", numNodes, nodeIDs,
				dofsFhat);
		GiDFileIO::appendNodalDataToDotRes(dataFileName, "\"mmsDTarget\"",
				"\"mms carat testing\"", i, "Vector", numNodes, nodeIDs,
				dofsDhat);


		GiDFileIO::appendNodalDataToDotRes(dataFileName, "\"mmsFDeviaion\"",
				"\"mms carat testing\"", i, "Vector", numNodes, nodeIDs,
				dofsFDev);
		GiDFileIO::appendNodalDataToDotRes(dataFileName, "\"mmsDDeviaion\"",
				"\"mms carat testing\"", i, "Vector", numNodes, nodeIDs,
				dofsDDev);
		GiDFileIO::appendNodalDataToDotRes(dataFileName, "\"mmsEDeviaion\"",
				"\"mms carat testing\"", i, "Vector", numNodes, nodeIDs,
				dofsEDev);


	}

	EMPIRE_API_Disconnect();
	probesF.close();
	probesD.close();
	probesE.close();
	return (0);
}

