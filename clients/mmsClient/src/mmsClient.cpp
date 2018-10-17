// set by rupert 2012/10/25

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
void mmsDeviation(double *recv, double *target, double *deviation, int numNodes) {
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

bool getBool(string text){
	if (text == "true" || text == "yes")
		return true;
	else if (text == "false" || text == "no")
		return false;
	else
		cout << "boolian input string is not valid: use true/false or yes/no"<< endl;
		exit (-1);

}

int getNumTimeSteps(string text) {
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
        exit(EXIT_FAILURE);
    }
    EMPIRE_API_Connect(argv[1]);

    bool todoIterativeCoupling = doIterativeCoupling(EMPIRE_API_getUserDefinedText("couplingType"));
    string meshfile = EMPIRE_API_getUserDefinedText("GiDMeshFile");
    string probefile = EMPIRE_API_getUserDefinedText("GiDProbeFile");
    string mmsFunction = EMPIRE_API_getUserDefinedText("mmsFunction");
    int numTimeSteps = getNumTimeSteps(EMPIRE_API_getUserDefinedText("numTimeSteps"));
    double timeStep = getDouble(EMPIRE_API_getUserDefinedText("timeStep"));
    double loadIncrement = getDouble(EMPIRE_API_getUserDefinedText("loadIncrement"));


    stringstream source;
    source << mmsFunction << "Source";
    stringstream target;
    target << mmsFunction << "Target";
    string mmsSource = source.str();
    string mmsTarget = target.str();

    int numNodes = -1;
    int numProbes = -1;
    int numElems = -1;
    int nodesPerElem = -1;
    int nodesPerElemP = -1;
    double loadFac= 0.0;
    double currentTime=0.0;
    int numCouplingIter=0;

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

    //sending probe file
    GiDFileIO::readDotMsh(probefile, numProbes, numElems, nodeCoorsP, nodeIDsP, numNodesPerElemP, elemTableP, elemIDsP);
    EMPIRE_API_sendMesh("defaultMesh", numProbes, numElems, nodeCoorsP, nodeIDsP, numNodesPerElemP, elemTableP);
    cout << "number of probes: " << numProbes <<endl;

    //sending mesh
    cout << "MMSClient: read mesh:" <<endl;
    GiDFileIO::readDotMsh(meshfile, numNodes, numElems, nodeCoors, nodeIDs, numNodesPerElem, elemTable, elemIDs);
    cout << "MMSClient: send mesh:" <<endl;
    EMPIRE_API_sendMesh("defaultMesh", numNodes, numElems, nodeCoors, nodeIDs, numNodesPerElem, elemTable);

    cout << "MMSClient: finalizing mesh:" <<endl;
    nodesPerElem = numNodesPerElem[0];
    nodesPerElemP = numNodesPerElemP[0];

    double *dofsSendTotal = new double[numNodes * 3];
    double *dofsSend = new double[numNodes * 3];
    double *dofsDiscreteForces = new double[numNodes * 3];
    double *dofsRecv = new double[numNodes * 3];
    double *dofsTargetTotal = new double[numNodes * 3];
    double *dofsTarget = new double[numNodes * 3];
    double *dofsDeviation = new double[numNodes * 3];
    double *dofsEnergyDeviation = new double[numNodes * 3];
    double *dofsProbeRecv = new double[numProbes *3];
    double *dofsProbeDev = new double[numProbes *3];
    double *dofsProbeEnergyDev = new double[numProbes *3];
    double *dofsProbeTarget = new double[numProbes *3];

    for (int j = 0; j < numNodes * 3; j++) {
    	dofsSendTotal[j]     = 0.0;
        dofsSend[j]          = 0.0;
        dofsDiscreteForces[j]= 0.0;
        dofsRecv[j]          = 0.0;
        dofsTarget[j]        = 0.0;
        dofsTargetTotal[j]   = 0.0;
        dofsDeviation[j]     = 0.0;
        dofsEnergyDeviation[j]=0.0;
    }
    for (int j = 0; j < numProbes * 3; j++) {
    	dofsProbeRecv[j]      = 0.000000000;
    	dofsProbeDev[j]	      = 0.000000000;
    	dofsProbeEnergyDev[j] = 0.000000000;
    }

    cout << "MCC: the mms Function is " << mmsFunction << endl;
    FieldCreator *fcSource = new FieldCreator(numNodes, numElems, nodesPerElem, nodeCoors, nodeIDs,elemTable, mmsSource);
    FieldCreator *fcTarget = new FieldCreator(numNodes, numElems, nodesPerElem, nodeCoors, nodeIDs,elemTable, mmsTarget);
    cout << "MCC: Number of " << nodesPerElem << "-node Elements: " << numElems <<endl;
    cout << "MCC: Number of Probes: " << numProbes <<endl;


    string dataFileName(meshfile);
    dataFileName.erase(dataFileName.size() - 4, 4);
    dataFileName.append(".res");

    string probeFileName(probefile);
    probeFileName.erase(probeFileName.size() - 4, 4);
    probeFileName.append(".res");


    GiDFileIO::initDotRes(dataFileName);

    //fcSource->create(dofsSend);
    //fcTarget->create(dofsTarget);
    
    ofstream probes ("probes");

    probes << "Timestep   ";
    for (int j=1; j <= numProbes;j++){
    probes <<j << "-dx \t \t \t " <<j << "-dy \t \t \t " <<j << "-dz \t \t \t ";
       }
    probes << endl;


    bool steadyState=0;
    if(loadIncrement != -1.0){
    	cout << "it's a steady simulation"  <<endl;
    	steadyState=1;
    	cout << "start"<<endl;
        fcSource->create(dofsSendTotal);
        cout << "creating force field finished" << endl;
        fcTarget->create(dofsTargetTotal);
    }



    //TIMELOOP
    cout << "Starting time loop with a time step size of " << timeStep << endl;
    for (int i = 1; i <= numTimeSteps; i++) {


    	currentTime=timeStep*double(i);
    	cout << "Current Time is " << currentTime << endl;

    	if (!steadyState){
        fcSource->create(dofsSend,currentTime);
        fcTarget->create(dofsTarget,currentTime);
    	}

    	else{ //if (steadyState){
    	if (loadFac<1.0)
    	loadFac+=loadIncrement;
    	else loadFac=1.0;
        cout<< "current load factor is " << loadFac << endl;

        if (loadFac<1.0){
        	for (int j = 0; j < numNodes * 3; j++) {
        		dofsSend[j]=dofsSendTotal[j]*loadFac;
        	}
        }
        else{
    	for (int j = 0; j < numNodes * 3; j++) {
        	dofsSend[j]=dofsSendTotal[j];
        	dofsTarget[j]=dofsTargetTotal[j];}
        }}


        if (!todoIterativeCoupling) {
            //LOOSE COUPLING

        	EMPIRE_API_sendDataField("defaultField", numNodes * 3, dofsSend);

        	EMPIRE_API_recvDataField("defaultField", numNodes * 3, dofsRecv);
        }
        else {
        	//ITERATIVE COUPLING
        	cout << "mmsClient: do iterative Coupling" <<endl;
            do {
                EMPIRE_API_sendDataField("defaultField", numNodes * 3, dofsSend);

                EMPIRE_API_recvDataField("defaultField", numNodes * 3, dofsRecv);

            } while (EMPIRE_API_recvConvergenceSignal() == 0);
        }

        if (loadFac==1.0 || steadyState==0){
        mmsDeviation(dofsRecv, dofsTarget, dofsDeviation, numNodes);
        }

        //map displacements
        EMPIRE_API_sendDataField("defaultField", numNodes * 3, dofsRecv);
        EMPIRE_API_recvDataField("defaultField", numProbes * 3, dofsProbeRecv);
        //map target
        //EMPIRE_API_sendDataField(numNodes * 3, dofsTarget);
        //EMPIRE_API_recvDataField(numProbes * 3, dofsProbeTarget);

        //map displacement deviations
        EMPIRE_API_sendDataField("defaultField", numNodes * 3, dofsDeviation);
        EMPIRE_API_recvDataField("defaultField", numProbes * 3, dofsProbeDev);

        //map energy deviations
        //EMPIRE_API_sendDataField(numNodes * 3, dofsEnergyDeviation);
        //EMPIRE_API_recvDataField(numProbes * 3, dofsProbeEnergyDev);

        //write deviation of displacements to file
        if (loadFac==1.0 || steadyState==0){
        probes << i<<" \t       ";
        for (int j=0; j <= 3*numProbes;j++){
         probes << dofsProbeDev[j] << " \t ";
           }
        probes << endl;
        }


        GiDFileIO::appendNodalDataToDotRes(dataFileName, "\"mmsDiscreteForces\"",
                  "\"mms carat testing\"", i, "Vector", numNodes, nodeIDs,  dofsDiscreteForces);
        GiDFileIO::appendNodalDataToDotRes(dataFileName, "\"mmsSource\"",
                "\"mms carat testing\"", i, "Vector", numNodes, nodeIDs,  dofsSend);
        GiDFileIO::appendNodalDataToDotRes(dataFileName, "\"mmsResults\"",
                "\"mms carat testing\"", i, "Vector", numNodes, nodeIDs,  dofsRecv);
        GiDFileIO::appendNodalDataToDotRes(dataFileName, "\"mmsTarget\"",
                "\"mms carat testing\"", i, "Vector", numNodes, nodeIDs,  dofsTarget);
        GiDFileIO::appendNodalDataToDotRes(dataFileName, "\"mmsDeviaion\"",
                "\"mms carat testing\"", i, "Vector", numNodes, nodeIDs, dofsDeviation);

        /*if (ownGidOutput)
        {
        	cout << "hello" <<endl;
         EMPIRE::GiDFileIO::appendResultToDotRes(probeFileName, "\"probeResults\"",
                "\"mms carat testing\"", i, "Vector", numProbes, nodeIDsP, true, dofsProbeRecv);
        EMPIRE::GiDFileIO::appendResultToDotRes(probeFileName, "\"probeDeviation\"",
                "\"mms carat testing\"", i, "Vector", numProbes, nodeIDsP, true, dofsProbeDev);
        }*/

    }

    EMPIRE_API_Disconnect();
    probes.close();
    return (0);
}

