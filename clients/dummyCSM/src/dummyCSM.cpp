#include "EMPIRE_API.h"
#include <assert.h>
#include "GiDFileIO.h"
#include "FluxCreator.h"
#include "FieldCreator.h"
#include <string>
#include <sstream>

using namespace std;
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

void caseNormal();
void caseT();
void caseTUM();
void caseMultipleMeshes();
/*
 * A dummy CSM solver doing loose coupling with OpenFoam. The number of time steps is 10.
 * It can generate the TUM logo which is punched into OpenFoam.
 */
int main(int argc, char **argv) {
    EMPIRE_API_Connect("dummyCSMInput.xml");

    string testCase = EMPIRE_API_getUserDefinedText("testCase");

    if (testCase == "normal") {
        caseNormal();
    } else if (testCase == "T") {
        caseT();
    } else if (testCase == "TUM") {
        caseTUM();
    } else if (testCase == "multipleMeshes") {
        caseMultipleMeshes();
    } else {
        assert(false);
    }
    EMPIRE_API_Disconnect();
    return (0);
}

void caseNormal() {
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
            elemTable, elemIDs);
    EMPIRE_API_sendMesh("defaultMesh", numNodes, numElems, nodeCoors, nodeIDs, numNodesPerElem, elemTable);
    int nodesPerElem = numNodesPerElem[0];
    bool todoIterativeCoupling = doIterativeCoupling(EMPIRE_API_getUserDefinedText("couplingType"));

    FieldCreator fc(numNodes, numElems, nodesPerElem, nodeCoors, nodeIDs, elemTable, "constant");
    for (int i = 1; i <= numTimeSteps; i++) {
        do {
            double dofsSend[numNodes * 3];
            fc.create(dofsSend);
            for (int j = 0; j < numNodes * 3; j++) {
                dofsSend[j] *= (i * 0.01);
            }

            for (int j = 0; j < numNodes; j++)
                dofsSend[j * 3 + 0] = 0.0; //Remove x
            for (int j = 0; j < numNodes; j++)
                dofsSend[j * 3 + 2] = 0.0; //Remove z

            EMPIRE_API_sendDataField("defaultField", numNodes * 3, dofsSend);
            //EMPIRE_API_printDataField("-----send-----", numNodes * 3, dofsSend);

            double dofsRecv[numNodes * 3];
            EMPIRE_API_recvDataField("defaultField", numNodes * 3, dofsRecv);
            //EMPIRE_API_printDataField("-----receive-----", numNodes * 3, dofsRecv);
            if (!todoIterativeCoupling)
                break;
        } while (EMPIRE_API_recvConvergenceSignal() == 0);
    }
    delete[] nodeCoors;
    delete[] nodeIDs;
    delete[] numNodesPerElem;
    delete[] elemTable;
    delete[] elemIDs;
}
void caseT() {
    const double value = 0.01;
    int numLogoNodes = 12;
    int logoNodes[] = { 4, 7, 8, 9, 13, 14, 15, 19, 20, 21, 22, 25 }; // numbers are visualized in GiD
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
            elemTable, elemIDs);
    EMPIRE_API_sendMesh("defaultMesh", numNodes, numElems, nodeCoors, nodeIDs, numNodesPerElem, elemTable);
    int nodesPerElem = numNodesPerElem[0];
    bool todoIterativeCoupling = doIterativeCoupling(EMPIRE_API_getUserDefinedText("couplingType"));

    for (int i = 1; i <= numTimeSteps; i++) {
        double dofsSend[numNodes * 3];
        for (int j = 0; j < numNodes * 3; j++)
            dofsSend[j] = 0.0;
        for (int j = 0; j < numLogoNodes; j++)
            dofsSend[(logoNodes[j] - 1) * 3 + 1] = i * value;

        EMPIRE_API_sendDataField("defaultField", numNodes * 3, dofsSend);
        EMPIRE_API_printDataField("-----send-----", numNodes * 3, dofsSend);

        double dofsRecv[numNodes * 3];
        EMPIRE_API_recvDataField("defaultField", numNodes * 3, dofsRecv);
        EMPIRE_API_printDataField("-----receive-----", numNodes * 3, dofsRecv);
    }
    delete[] nodeCoors;
    delete[] nodeIDs;
    delete[] numNodesPerElem;
    delete[] elemTable;
    delete[] elemIDs;
}
void caseTUM() {
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
            elemTable, elemIDs);
    EMPIRE_API_sendMesh("defaultMesh", numNodes, numElems, nodeCoors, nodeIDs, numNodesPerElem, elemTable);
    int nodesPerElem = numNodesPerElem[0];
    bool todoIterativeCoupling = doIterativeCoupling(EMPIRE_API_getUserDefinedText("couplingType"));

    const double value = 0.01;
    int logoNodes[] = { 37, 48, 38, 52, 42, 54, 67, 82, 97, 116, 135, 49, 59, 70, 85, 101, 121, 139,
            55, 69, 65, 79, 77, 86, 99, 115, 132, 149, 163, 88, 100, 113, 126, 142, 161, 180, 175,
            192, 187, 208, 136, 150, 160, 174, 186, 206, 226, 155, 164, 181, 193, 207, 228, 239,
            176, 185, 198, 210, 219, 236, 252, 266, 290, 223, 234, 240, 260, 274, 292, 309, 248,
            259, 273, 284, 295, 311, 325, 336, 348, 299, 316, 323, 335, 342, 353, 362 }; // numbers are visualized in GiD
    int numLogoNodes = sizeof(logoNodes) / sizeof(int);

    string resFileName = meshfile.erase(meshfile.size() - 3, 3);
    resFileName.append("res");
    GiDFileIO::initDotRes(resFileName);

    for (int i = 1; i <= numTimeSteps; i++) {
        double dofsSend[numNodes * 3];
        for (int j = 0; j < numNodes * 3; j++)
            dofsSend[j] = 0.0;
        for (int j = 0; j < numLogoNodes; j++)
            dofsSend[(logoNodes[j] - 1) * 3 + 1] = i * value;

        EMPIRE_API_sendDataField("defaultField", numNodes * 3, dofsSend);
        EMPIRE_API_printDataField("-----send-----", numNodes * 3, dofsSend);
        GiDFileIO::appendNodalDataToDotRes(resFileName, "\"displacements\"", "\"TUM_logo\"",
                i, "Vector", numNodes, nodeIDs, dofsSend);

        double dofsRecv[numNodes * 3];
        EMPIRE_API_recvDataField("defaultField", numNodes * 3, dofsRecv);
        EMPIRE_API_printDataField("-----receive-----", numNodes * 3, dofsRecv);
        GiDFileIO::appendNodalDataToDotRes(resFileName, "\"forces\"", "\"TUM_logo\"", i,
                "Vector", numNodes, nodeIDs, dofsSend);
    }
    delete[] nodeCoors;
    delete[] nodeIDs;
    delete[] numNodesPerElem;
    delete[] elemTable;
    delete[] elemIDs;
}
void caseMultipleMeshes() {
    string meshfile1 = EMPIRE_API_getUserDefinedText("GiDMeshFile1");
    string meshfile2 = EMPIRE_API_getUserDefinedText("GiDMeshFile2");

    int numNodes1, numNodes2;
    int numElems1, numElems2;
    double *nodeCoors1, *nodeCoors2;
    int *nodeIDs1, *nodeIDs2;
    int *numNodesPerElem1, *numNodesPerElem2;
    int *elemTable1, *elemTable2;
    int *elemIDs1, *elemIDs2;
    GiDFileIO::readDotMsh(meshfile1, numNodes1, numElems1, nodeCoors1, nodeIDs1,
            numNodesPerElem1, elemTable1, elemIDs1);
    EMPIRE_API_sendMesh("defaultMesh", numNodes1, numElems1, nodeCoors1, nodeIDs1, numNodesPerElem1, elemTable1);
    int nodesPerElem1 = numNodesPerElem1[0];

    GiDFileIO::readDotMsh(meshfile2, numNodes2, numElems2, nodeCoors2, nodeIDs2,
            numNodesPerElem2, elemTable2, elemIDs2);
    EMPIRE_API_sendMesh("defaultMesh", numNodes2, numElems2, nodeCoors2, nodeIDs2, numNodesPerElem2, elemTable2);
    int nodesPerElem2 = numNodesPerElem2[0];

    int numTimeSteps = getNumTimeSteps(EMPIRE_API_getUserDefinedText("numTimeSteps"));
    bool todoIterativeCoupling = doIterativeCoupling(EMPIRE_API_getUserDefinedText("couplingType"));

    for (int i = 1; i <= numTimeSteps; i++) {
        do {
            double dofsSend1[numNodes1 * 3];
            for (int j = 0; j < numNodes1; j++) {
                dofsSend1[j * 3 + 0] = 0.0;
                dofsSend1[j * 3 + 1] = (i * 0.01);
                dofsSend1[j * 3 + 2] = 0.0;
            }
            double dofsSend2[numNodes2 * 3];
            for (int j = 0; j < numNodes2 * 3; j++) {
                dofsSend2[j] = 0.0;
            }

            EMPIRE_API_sendDataField("defaultField", numNodes1 * 3, dofsSend1);
            EMPIRE_API_sendDataField("defaultField", numNodes2 * 3, dofsSend2);
            //EMPIRE_API_printDataField("-----send-----", numNodes * 3, dofsSend);

            double dofsRecv1[numNodes1 * 3];
            double dofsRecv2[numNodes2 * 3];
            EMPIRE_API_recvDataField("defaultField", numNodes1 * 3, dofsRecv1);
            EMPIRE_API_recvDataField("defaultField", numNodes2 * 3, dofsRecv2);
            //EMPIRE_API_printDataField("-----receive-----", numNodes * 3, dofsRecv);
            if (!todoIterativeCoupling)
                break;
        } while (EMPIRE_API_recvConvergenceSignal() == 0);
    }
    delete[] nodeCoors1;
    delete[] nodeIDs1;
    delete[] numNodesPerElem1;
    delete[] elemTable1;
    delete[] elemIDs1;
    delete[] nodeCoors2;
    delete[] nodeIDs2;
    delete[] numNodesPerElem2;
    delete[] elemTable2;
    delete[] elemIDs2;
}
