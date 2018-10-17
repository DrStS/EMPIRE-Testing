empireBaseDir=getenv('EMPIRE_BASE_DIR');
tmpDir='/interface/matlab/binICC';
mexFuncPath=strcat(empireBaseDir, tmpDir);
path(path, mexFuncPath);


EMPIRE_API_Connect('matlabClient.xml');
text = EMPIRE_API_getUserDefinedText('word');
display(text);


numNodes=4;
numElems=1;
nodes=[0,0,0;1,0,0;1,1,0;0,1,0];
nodeIDs=[1,2,3,4];
numNodesPerElem=[4];
elems=[1,2,3,4];
EMPIRE_API_sendMesh(numNodes, numElems, nodes', nodeIDs, numNodesPerElem, elems);

signal1=[1,2,3,4,5];
EMPIRE_API_sendSignal_double('signal1', 5, signal1);
EMPIRE_API_recvSignal_double('signal1', 5, signal1);

df1=[0,0,1;0,0,2;0,0,3;0,0,4];
EMPIRE_API_sendDataField(12, df1');
EMPIRE_API_recvDataField(12, df1');

df1;

EMPIRE_API_Disconnect();

clear functions;
