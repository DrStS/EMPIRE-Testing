/*
 * Mesh.h
 *
 *  Created on: May 20, 2014
 *      Author: fabien
 */
/***********************************************************************************************
 * \file Mesh.h
 * This file holds the class Mesh
 * \date 15/06/2014
 **************************************************************************************************/
#ifndef MESH_H_
#define MESH_H_
#include <string>
#include <vector>
#include "MeshInterface.h"

namespace optclient {
/********//**
 * \brief Class Mesh for handling classical FEM mesh from GiD format
 ***********/
class Mesh : public MeshInterface {
public:
	/*CONSTRUCTOR/DESTRUCTOR*/
	Mesh(std::string fileName_);
	virtual ~Mesh();

	/*INHERITED INTERFACE*/
	double* getMeshNodes(){return meshNodeCoordinates;};
	int getMeshNodeNumber(){return numberOfMeshNodes;};
private:
	void read();
	void send();
	void displayInfo() {};

	/*MESH DATA*/
	std::string fileName;
	int numberOfMeshNodes;
    double *meshNodeCoordinates;
    int numberOfElements;
    int *meshNodeIds;
    int *numberOfNodesPerElement;
    int *elementNodeTables;
    int *elementIds;
};

} /* namespace optclient */
#endif /* MESH_H_ */
