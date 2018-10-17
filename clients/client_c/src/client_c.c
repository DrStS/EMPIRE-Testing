// This is the client
// Using Intel MPI 4.0.3
// Starting the server with (mpdboot running) 
//   mpiexec -np 1 ./server

#include "EMPIRE_API.h"
#include <assert.h>

int main( int argc, char **argv ) 
{   
  EMPIRE_API_Connect("empireClient_c.xml");

  const int numNodes = 10;
  int numElems = 1;
  double nodes[] = {
          0,0,0,
          1,0,0,
          1,1,0,
          0,1,0,
          0,0,0,
          0,0,0,
          0,0,0,
          0,0,0,
          0,0,0,
          0,0,0
  };
  int elems[] = {1,2,3,4,5,6,7,8,9,10};
  int nodeIDs[] = {1,2,3,4,5,6,7,8,9,10};
  int numNodesPerElem[] = {10};

  EMPIRE_API_sendMesh("defaultMesh" ,numNodes, numElems, nodes, nodeIDs, numNodesPerElem, elems);

  const int fieldSize2 = numNodes * 3;
  double field2[fieldSize2];
  EMPIRE_API_recvDataField("defaultField", fieldSize2, field2);

  EMPIRE_API_printDataField("-----receive-----", fieldSize2, field2);

  const int fieldSize = numNodes;
  double field[] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};
  EMPIRE_API_sendDataField("defaultField", fieldSize, field);

  EMPIRE_API_printDataField("-----send-----", fieldSize, field);


  EMPIRE_API_Disconnect();

  return (0);
}  


























