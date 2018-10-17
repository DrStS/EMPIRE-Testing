// This is the client
// Using Intel MPI 4.0.3
// Starting the server with (mpdboot running) 
//   mpiexec -np 1 ./server

#include "EMPIRE_API.h"
#include <assert.h>

int main(int argc, char **argv) {

    EMPIRE_API_Connect("empireClient_cpp.xml");

    const int numNodes = 4;
    int numElems = 1;
    double nodes[] = {
            0,0,0,
            1,0,0,
            1,1,0,
            0,1,0
    };
    int nodeIDs[] = {1,2,3,4};
    int elems[] = {
            1,2,3,4
    };
    int numNodesPerElem[] = {4};

    EMPIRE_API_sendMesh("defaultMesh", numNodes, numElems, nodes, nodeIDs, numNodesPerElem, elems);

    const int fieldSize = numNodes * 3;
    double field[] = {1.1, 1.2, 1.3, 2.1, 2.2, 2.3, 3.1, 3.2, 3.3, 4.1, 4.2, 4.3};
    EMPIRE_API_sendDataField("defaultField", fieldSize, field);

    EMPIRE_API_printDataField("-----send-----", fieldSize, field);

    const int fieldSize2 = numNodes;
    double field2[fieldSize2];
    EMPIRE_API_recvDataField("defaultField", fieldSize2, field2);

    EMPIRE_API_printDataField("-----receive-----", fieldSize2, field2);


    EMPIRE_API_Disconnect();

    return (0);
}

