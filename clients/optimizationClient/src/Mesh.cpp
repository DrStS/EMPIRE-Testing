/*
 * Mesh.cpp
 *
 *  Created on: May 20, 2014
 *      Author: fabien
 */

#include "Mesh.h"
#include "GiDFileIO.h"
#include "EMPIRE_API.h"
#include <assert.h>

namespace optclient {

Mesh::Mesh(std::string fileName_):
		fileName(fileName_) {
	read();
	send();
}

Mesh::~Mesh() {
	delete[] meshNodeCoordinates;
	delete[] meshNodeIds;
	delete[] numberOfNodesPerElement;
	delete[] elementNodeTables;
	delete[] elementIds;
}

void Mesh::read() {
	GiDFileIO::readDotMsh(fileName, numberOfMeshNodes, numberOfElements,
	        meshNodeCoordinates, meshNodeIds, numberOfNodesPerElement,elementNodeTables, elementIds);
}

void Mesh::send() {
	EMPIRE_API_sendMesh("",numberOfMeshNodes,numberOfElements,meshNodeCoordinates,meshNodeIds,numberOfNodesPerElement,elementNodeTables);
}

} /* namespace optclient */
