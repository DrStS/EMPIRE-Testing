#include "EMPIRE_API.h"
#include <assert.h>
#include "GiDFileIO.h"
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

void convertToLowerCase(string &str);

bool doIterativeCoupling(string text) {
    convertToLowerCase(text);
    if (text == "iterativecoupling")
        return true;
    else if (text == "loosecoupling")
        return false;
    else
        assert(false);
    return false;
}

bool onNodes(string text) {
    convertToLowerCase(text);
    if (text == "onnodes")
        return true;
    else if (text == "ongausspoints")
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

int getTimeInterval(string text) {
    stringstream ss(text);
    int timeInterval;
    ss >> timeInterval;
    return timeInterval;
}

/*
 * RecorderClient reads GiD .msh and .res file, send result at each timestep to the server
 */
int main(int argc, char **argv) {
    EMPIRE_API_Connect("recorderClientInput.xml");

    int numTimeSteps = getNumTimeSteps(EMPIRE_API_getUserDefinedText("numTimeSteps"));
    int timeInterval = getNumTimeSteps(EMPIRE_API_getUserDefinedText("timeInterval"));
    string meshfile = EMPIRE_API_getUserDefinedText("GiDMeshFile");
    string resfileName = EMPIRE_API_getUserDefinedText("GiDResultFile");
    string resultName = EMPIRE_API_getUserDefinedText("GiDResultFile_resultName");
    string analysisName = EMPIRE_API_getUserDefinedText("GiDResultFile_analysisName");
    bool onNodes = EMPIRE_API_getUserDefinedText("GiDResultFile_dataFieldLocation");

    int numNodes = -1;
    int numElems = -1;
    double *nodeCoors;
    int *nodeIDs;
    int *numNodesPerElem;
    int *elemTable;
    int *elemIDs;
    GiDFileIO::readDotMsh(meshfile, numNodes, numElems, nodeCoors, nodeIDs, numNodesPerElem,
            elemTable, elemIDs);
    EMPIRE_API_sendMesh("defaultMesh", numNodes, numElems, nodeCoors, nodeIDs, numNodesPerElem,
            elemTable);
    bool todoIterativeCoupling = doIterativeCoupling(EMPIRE_API_getUserDefinedText("couplingType"));

    double *nodalData = new double[numNodes * 3];
    double *elementalData = new double[numElems * 3];

    ifstream dotResFile(resfileName.c_str());

    for (int i = 1; i <= numTimeSteps; i++) {
        do {
            if (onNodes) {
                GiDFileIO::readNodalDataFromDotResFast(dotResFile, resfileName, resultName, analysisName,
                        i * timeInterval, "vector", numNodes, nodeIDs, nodalData);
                EMPIRE_API_sendDataField("defaultField", numNodes * 3, nodalData);
            } else {
                GiDFileIO::readElementalDataFromDotResFast(dotResFile, resfileName, resultName, analysisName,
                        i * timeInterval, "vector", numElems, elemIDs, numNodesPerElem,
                        elementalData);
                EMPIRE_API_sendDataField("defaultField", numElems * 3, elementalData);
            }

            // do not receive any data
            if (!todoIterativeCoupling)
                break;
        } while (EMPIRE_API_recvConvergenceSignal() == 0);
    }

    delete[] nodalData;
    delete[] elementalData;
    delete[] nodeCoors;
    delete[] nodeIDs;
    delete[] numNodesPerElem;
    delete[] elemTable;
    delete[] elemIDs;

    EMPIRE_API_Disconnect();
    return (0);
}

/***********************************************************************************************
 * \brief Convert the string to a lower case string
 * \param[in] str the string
 * \author Tianyang Wang
 ***********/
void convertToLowerCase(string &str) {
    for (unsigned i = 0; i < str.size(); i++) {
        str[i] = tolower(str[i]);
    }
}
