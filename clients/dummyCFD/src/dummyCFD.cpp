#include "EMPIRE_API.h"
#include <assert.h>
#include "GiDFileIO.h"
#include "FluxCreator.h"
#include "FieldCreator.h"
#include <string>
#include <sstream>
#include <set>
#include "LoadCurveParser.h"
#include <stdlib.h>

using namespace std;
bool doIterativeCoupling() {
    string text = EMPIRE_API_getUserDefinedText("couplingType");
    if (text == "iterativeCoupling")
        return true;
    else if (text == "looseCoupling")
        return false;
    else
        assert(false);
    return false;
}

int getNumTimeSteps() {
    string text = EMPIRE_API_getUserDefinedText("numTimeSteps");
    stringstream ss(text);
    int numTimeSteps;
    ss >> numTimeSteps;
    return numTimeSteps;
}

void getLoadNodeIDs(set<int> &loadNodeIDs) {
    string text = EMPIRE_API_getUserDefinedText("loadNodeIDs");
    stringstream ss(text);
    int nodeID;
    while (ss >> nodeID) {
        loadNodeIDs.insert(nodeID);
    }
}

int getLoadDirection() {
    string text = EMPIRE_API_getUserDefinedText("loadDirection");
    stringstream ss(text);
    int direction;
    ss >> direction;
    return direction;
}

double getTimeStepLength() {
    string text = EMPIRE_API_getUserDefinedText("timeStepLength");
    stringstream ss(text);
    double timeStepLength;
    ss >> timeStepLength;
    return timeStepLength;
}

double getStartTime() {
    string text = EMPIRE_API_getUserDefinedText("startTime");
    stringstream ss(text);
    double startTime;
    ss >> startTime;
    return startTime;
}

/*
 * A dummy CFD solver, which is used to do coupling with carat.
 * It reads the load curve of some carat input file, sends the load to carat.
 * Therefore, we expect the same result as running a single carat program on the same carat input file.
 */
int main(int argc, char **argv) {
    if (argc != 2) {
        cerr << "Error: Please provide a valid input file." << endl;
        exit(EXIT_FAILURE);
    }
    EMPIRE_API_Connect(argv[1]);

    bool todoIterativeCoupling = doIterativeCoupling();
    string meshfile = EMPIRE_API_getUserDefinedText("GiDMeshFile");
    int numTimeSteps = getNumTimeSteps();
    string loadCurveFile = EMPIRE_API_getUserDefinedText("loadCurveFile");
    LoadCurveParser loadCurveParser(loadCurveFile);

    int numNodes;
    int numElems;
    double *nodeCoors;
    int *nodeIDs;
    int *numNodesPerElem;
    int *elemTable;
    int *elemIDs;
    GiDFileIO::readDotMsh(meshfile, numNodes, numElems, nodeCoors, nodeIDs, numNodesPerElem,
				  elemTable,elemIDs);
    EMPIRE_API_sendMesh("defaultField", numNodes, numElems, nodeCoors, nodeIDs, numNodesPerElem, elemTable);

    double *dofsSend = new double[numNodes * 3];
    double *dofsRecv = new double[numNodes * 3];

    double currentTime = getStartTime();
    set<int> loadNodeIDs;
    getLoadNodeIDs(loadNodeIDs);
    int loadDirection = getLoadDirection();
    for (int i = 1; i <= numTimeSteps; i++) {
        currentTime = currentTime + getTimeStepLength();
        double load = loadCurveParser.getValueAtTime(currentTime);
        if (!todoIterativeCoupling) {
            { // block of creating fluid load by load curve
                for (int j = 0; j < numNodes * 3; j++) {
                    dofsSend[j] = 0.0;
                }
                for (int j = 0; j < numNodes; j++)
                    if (loadNodeIDs.find(nodeIDs[j]) != loadNodeIDs.end())
                        dofsSend[j * 3 + loadDirection] = load;
            }

            EMPIRE_API_sendDataField("defaultField", numNodes * 3, dofsSend);
            //EMPIRE_API_printDataField("-----send-----", numNodes * 3, dofsSend);

            EMPIRE_API_recvDataField("defaultField", numNodes * 3, dofsRecv);
            //EMPIRE_API_printDataField("-----receive-----", numNodes * 3, dofsRecv);
        } else {
            do {
                { // block of creating fluid load by load curve
                    for (int j = 0; j < numNodes * 3; j++) {
                        dofsSend[j] = 0.0;
                    }
                    for (int j = 0; j < numNodes; j++)
                        if (loadNodeIDs.find(nodeIDs[j]) != loadNodeIDs.end())
                            dofsSend[j * 3 + loadDirection] = load;
                }

                EMPIRE_API_sendDataField("defaultField", numNodes * 3, dofsSend);
                //EMPIRE_API_printDataField("-----send-----", numNodes * 3, dofsSend);

                EMPIRE_API_recvDataField("defaultField", numNodes * 3, dofsRecv);
                //EMPIRE_API_printDataField("-----receive-----", numNodes * 3, dofsRecv);
            } while (EMPIRE_API_recvConvergenceSignal() == 0);

        }
    }

    EMPIRE_API_Disconnect();
    delete[] nodeCoors;
    delete[] nodeIDs;
    delete[] numNodesPerElem;
    delete[] elemTable;
    delete[] elemIDs;
    delete[] dofsRecv;
    delete[] dofsSend;
    return (0);
}

