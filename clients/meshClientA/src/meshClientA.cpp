#include "EMPIRE_API.h"
#include <assert.h>
#include "GiDFileIO.h"
#include "FluxCreator.h"
#include "FieldCreator.h"
#include <string>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <math.h>

using namespace std;

void dummyUpdataPDE() {
    // t = t + delta_t;
    // set up the PDE at this time step
}
void dummySolvePDE() {
    // solve the PDE
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

int getNumTimeSteps(string text) {
    stringstream ss(text);
    int numTimeSteps;
    ss >> numTimeSteps;
    return numTimeSteps;
}
/*
 * It is coupled with meshClientB.
 * A dummy client which reads gid mesh, sends constant data field to the server.
 * If time step is 3, then in loose coupling, the data field sent is (3.0, 3.0, 3.0),
 * or in iterative coupling, the data field sent is (3.01, 3.01, 3.01) if meanwhile it is
 * at the 2nd iterative loop.
 */
int main(int argc, char **argv) {
    if (argc != 2) {
        cerr << "Error: Please provide a valid input file." << endl;
        exit(EXIT_FAILURE);
    }
    EMPIRE_API_Connect(argv[1]);

    bool todoIterativeCoupling = doIterativeCoupling(EMPIRE_API_getUserDefinedText("couplingType"));
    string meshfile = EMPIRE_API_getUserDefinedText("GiDMeshFile");
    int numTimeSteps = getNumTimeSteps(EMPIRE_API_getUserDefinedText("numTimeSteps"));

    int numNodes = -1;
    int numElems = -1;
    double *nodeCoors;
    int *nodeIDs;
    int *numNodesPerElem;
    int *elemTable;
    int *elemIDs;
    GiDFileIO::readDotMsh(meshfile, numNodes, numElems, nodeCoors, nodeIDs, numNodesPerElem,
				  elemTable,elemIDs);
    EMPIRE_API_sendMesh("defaultMesh", numNodes, numElems, nodeCoors, nodeIDs, numNodesPerElem, elemTable);

    double *dofsSend = new double[numNodes * 3];
    for (int j = 0; j < numNodes * 3; j++)
        dofsSend[j] = 0.0;
    double *dofsRecv = new double[numNodes * 3];
    for (int j = 0; j < numNodes * 3; j++)
        dofsRecv[j] = 0.0;

    FieldCreator *fc = new FieldCreator(numNodes, numElems, numNodesPerElem[0], nodeCoors, nodeIDs,
            elemTable, "constant");
    for (int i = 1; i <= numTimeSteps; i++) {
        if (!todoIterativeCoupling) {
            /*for (int j = 0; j < numNodes; j++)
             dofsSend[j * 3 + 2] = i;*/
            fc->create(dofsSend);
            for (int j = 0; j < numNodes * 3; j++)
                dofsSend[j] *= i;
            cout << "sending ..." << endl;
            EMPIRE_API_sendDataField("defaultField", numNodes * 3, dofsSend);
            //EMPIRE_API_printDataField("-----send-----", numNodes * 3, dofsSend);

            cout << "receiving ..." << endl;
            EMPIRE_API_recvDataField("defaultField", numNodes * 3, dofsRecv);
            //EMPIRE_API_printDataField("-----receive-----", numNodes * 3, dofsRecv);

            dummyUpdataPDE();
            dummySolvePDE();
        } else {
            double solution = (double) i;
            for (int j = 0; j < numNodes; j++)
                dofsSend[j * 3 + 2] = solution;

            int count = 0;
            do {
                count++;
                for (int j = 0; j < numNodes; j++)
                    dofsSend[j * 3 + 2] = solution + pow(10, (double) (-count));
                cout << "sending ..." << endl;
                EMPIRE_API_sendDataField("defaultField", numNodes * 3, dofsSend);
                //EMPIRE_API_printDataField("-----send-----", numNodes * 3, dofsSend);
                cout << "receiving ..." << endl;
                EMPIRE_API_recvDataField("defaultField", numNodes * 3, dofsRecv);
                //EMPIRE_API_printDataField("-----receive-----", numNodes * 3, dofsRecv);
            } while (EMPIRE_API_recvConvergenceSignal() == 0);
            /*std::cout << std::endl;
             std::cout << "Time step: " << i << ", no. of inner loop steps: " << count
             << std::endl;*/
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

