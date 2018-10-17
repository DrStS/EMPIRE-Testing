/*
 * MeshIGA.cpp
 *
 *  Created on: Jun 11, 2014
 *      Author: fabien
 */

#include "MeshIGA.h"
#include "RhinoFileIO.h"
#include <EMPIRE_API.h>
#include <iostream>

namespace optclient {

MeshIGA::MeshIGA(std::string fileName_) : fileName(fileName_), numberOfMeshNodes(0), numberOfPatches(0) {
	read();
	send();
}

MeshIGA::~MeshIGA() {
	for(int i = 0; i<numberOfPatches;i++) {
		delete patches[i];
	}
}

void MeshIGA::read(){
	numberOfPatches=0;numberOfMeshNodes=0;
	readMetaData(fileName, numberOfPatches,meshNodesCoordinates);
	numberOfMeshNodes=meshNodesCoordinates.size()/3;
	for(int i = 0; i< numberOfPatches;i++) {
		patches.push_back(new NurbsPatch(this));
		patches[i]->patchID=i+1;
		patches[i]->read();
	}
}
void MeshIGA::send(){
	EMPIRE_API_sendIGAMesh("",numberOfPatches,numberOfMeshNodes);
	std::cout<<numberOfPatches<<" "<<numberOfMeshNodes<<std::endl;
	for(int i=0; i<numberOfPatches;i++) {
		patches[i]->send();
	}
	displayInfo();
}

void MeshIGA::NurbsPatch::read() {
	readNurbsPatch(mesh->getFileName(),patchID,type,
			pDegree,uKnotVector,
			qDegree,vKnotVector,
			uNumControlPoints, vNumControlPoints,
			nodeNet, weights);
}

void MeshIGA::NurbsPatch::send() {
	double* meshNodesCoordinates=mesh->getMeshNodes();
	std::vector<double> cpNet;
	int k=0;
	/// create control point net by copying all coordinates of the nodes present in the patch
        for(std::vector<int>::iterator j=nodeNet.begin(); j!=nodeNet.end();j++) {
		cpNet.push_back(meshNodesCoordinates[(*j)*3+0]);
		cpNet.push_back(meshNodesCoordinates[(*j)*3+1]);
		cpNet.push_back(meshNodesCoordinates[(*j)*3+2]);
		cpNet.push_back(weights[k]);
		k++;
	}
	EMPIRE_API_sendIGAPatch(
			pDegree,uKnotVector.size(),uKnotVector.data(),
			qDegree,vKnotVector.size(),vKnotVector.data(),
			uNumControlPoints, vNumControlPoints,
			cpNet.data(), nodeNet.data());
}

void MeshIGA::displayInfo() {
	using namespace std;
	cout<<"/////INFO MESH/////"<<endl;
	cout<<"#mesh nodes : "<<meshNodesCoordinates.size()/3<<endl;
	for(std::vector<double>::iterator i=meshNodesCoordinates.begin();i!=meshNodesCoordinates.end();i+=3)
		cout<<"\t X "<<*i<<" Y "<<*(i+1)<<" Z "<<*(i+2)<<endl;
	cout<<"#patch : "<<patches.size()<<endl;
	for(unsigned int i=0; i<patches.size();i++) {
		patches[i]->displayInfo();
	}
}
void MeshIGA::NurbsPatch::displayInfo() {
	using namespace std;
		cout<<"/////INFO Patch #"<<patchID<<"/////"<<endl;
		cout<<"\t Mesh type : "<<type<<endl;
		cout<<"\t uKnot size : "<<uKnotVector.size()<<endl;
		cout<<"\t vKnot size : "<<vKnotVector.size()<<endl;
		cout<<"\t pDeg : "<<pDegree<<endl;
		cout<<"\t qDeg : "<<qDegree<<endl;
		cout<<"\t u ctrl pts : "<<uNumControlPoints<<endl;
		cout<<"\t v ctrl pts : "<<vNumControlPoints<<endl;
		cout<<"\t NodeNet : "<<nodeNet.size()<<endl;
		cout<<"\t \t ";
                for(std::vector<int>::iterator i=nodeNet.begin();i!=nodeNet.end();i++)
			cout<<*i<<" ";
		cout<<endl;
		cout<<"\t Weights : "<<weights.size()<<endl;
		cout<<"\t \t ";
                for(std::vector<double>::iterator i=weights.begin();i!=weights.end();i++)
			cout<<*i<<" ";
		cout<<endl;
		cout<<"//////////"<<endl;
}

} /* namespace optclient */
