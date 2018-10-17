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

enum Scheme {
    Gauss_Seidel, Jacobi
} scheme = Jacobi;

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
 * It is coupled with meshClientA.
 * A dummy client which reads gid mesh, sends consistent nodal forces to the server.
 * The consistent nodal force is computed from a constant pressure field over the surface.
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

    int numNodes;
    int numElems;
    double *nodeCoors;
    int *nodeIDs;
    int *numNodesPerElem;
    int *elemTable;
    int *elemIDs;

    GiDFileIO::readDotMsh(meshfile, numNodes, numElems, nodeCoors, nodeIDs, numNodesPerElem,
				  elemTable, elemIDs);

    EMPIRE_API_sendMesh("defaultMesh", numNodes, numElems, nodeCoors, nodeIDs, numNodesPerElem, elemTable);

    FluxCreator *fc = new FluxCreator(numNodes, numElems, numNodesPerElem[0], nodeCoors, nodeIDs,
            elemTable, "constant", true);

    bool sendTractionElem = false;
    string sendDataField = EMPIRE_API_getUserDefinedText("sendDataField");
    if (sendDataField == "traction")
        sendTractionElem = true;
    else if (sendDataField == "force")
        sendTractionElem = false;
    else
        assert(false);

    double *dofsSend;
    if (!sendTractionElem) {
        dofsSend = new double[numNodes * 3];
        for (int j = 0; j < numNodes * 3; j++)
            dofsSend[j] = 0.0;
    } else {
        dofsSend = new double[numElems * 3];
        for (int j = 0; j < numElems * 3; j++)
            dofsSend[j] = 0.0;
    }
    double *dofsRecv = new double[numNodes * 3];
    for (int j = 0; j < numNodes * 3; j++)
        dofsRecv[j] = 0.0;

    for (int i = 1; i <= numTimeSteps; i++) {
        if (!todoIterativeCoupling) {
            cout << "receiving ..." << endl;
            EMPIRE_API_recvDataField("defaultField", numNodes * 3, dofsRecv);
            //EMPIRE_API_printDataField("-----receive-----", numNodes * 3, dofsRecv);

            if (scheme == Gauss_Seidel) {
                dummyUpdataPDE();
                dummySolvePDE();
            }

            if (!sendTractionElem) {
                fc->create(dofsSend);
                for (int j = 0; j < numNodes * 3; j++)
                    dofsSend[j] *= i;
                cout << "sending ..." << endl;
                EMPIRE_API_sendDataField("defaultField", numNodes * 3, dofsSend);
                //EMPIRE_API_printDataField("-----send-----", numNodes * 3, dofsSend);
            } else {
                for (int j = 0; j < numElems; j++)
                    dofsSend[j * 3 + 2] = i;
                cout << "sending ..." << endl;
                EMPIRE_API_sendDataField("defaultField", numElems * 3, dofsSend);
                //EMPIRE_API_printDataField("-----send-----", numElems * 3, dofsSend);
            }

            if (scheme == Jacobi) {
                dummyUpdataPDE();
                dummySolvePDE();
            }
        } else {
            do {
                cout << "receiving ..." << endl;
                EMPIRE_API_recvDataField("defaultField", numNodes * 3, dofsRecv);
                //EMPIRE_API_printDataField("-----receive-----", numNodes * 3, dofsRecv);

                if (!sendTractionElem) {
                    fc->create(dofsSend);
                    for (int j = 0; j < numNodes; j++)
                        dofsSend[j * 3 + 2] *= i;
                    cout << "sending ..." << endl;
                    EMPIRE_API_sendDataField("defaultField", numNodes * 3, dofsSend);
                    //EMPIRE_API_printDataField("-----send-----", numNodes * 3, dofsSend);
                } else {
                    for (int j = 0; j < numElems; j++)
                        dofsSend[j * 3 + 2] = i;
                    cout << "sending ..." << endl;
                    EMPIRE_API_sendDataField("defaultField", numElems * 3, dofsSend);
                    //EMPIRE_API_printDataField("-----send-----", numElems * 3, dofsSend);
                }
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

