/*
 * MeshIGA.h
 *
 *  Created on: Jun 11, 2014
 *      Author: fabien
 */
/***********************************************************************************************
 * \file MeshIGA.h
 * This file holds the class MeshIGA and MeshIGA::NurbPatch
 * \date 15/06/2014
 **************************************************************************************************/
#ifndef MESHIGA_H_
#define MESHIGA_H_
#include "MeshInterface.h"
#include <string>
#include <vector>

namespace optclient {
/********//**
 * \brief Class MeshIGA for handling Nurbs mesh from Rhino format
 ***********/
class MeshIGA : public MeshInterface {
public:
	/*CONSTRUCTOR/DESTRUCTOR*/
	MeshIGA(std::string fileName_);
	virtual ~MeshIGA();

	/*CLASS METHODS*/
	const std::string getFileName(){return fileName;};
	const MeshInterface* getPatch(int i) {return (const MeshInterface*)patches.at(i);};

	/*INHERITED INTERFACE*/
	double* getMeshNodes(){return meshNodesCoordinates.data();};
	const double* getMeshNodes() const {return meshNodesCoordinates.data();};
	int getMeshNodeNumber(){return numberOfMeshNodes;};
private:
	void read();
	void send();
	void displayInfo();

private:
	/*MESH DATA*/
	std::string fileName;
	int numberOfMeshNodes;
	std::vector<double> meshNodesCoordinates;
	int numberOfPatches;

    class NurbsPatch;
	std::vector<NurbsPatch*> patches;
};

/********//**
 * \brief Class NurbsPatch for handling data from a single patch
 ***********/
class MeshIGA::NurbsPatch : public MeshInterface {
public:
	/*CONSTRUCTOR/DESTRUCTOR*/
	NurbsPatch(MeshIGA* m):mesh(m){};
	~NurbsPatch(){};

	/*INHERITED INTERFACE*/
	double* getMeshNodes(){return mesh->getMeshNodes();};
	int getMeshNodeNumber(){return mesh->getMeshNodeNumber();};
private:
	void read();
	void send();
	void displayInfo();

private:
	/*PATCH DATA*/
	MeshIGA* mesh;
	friend class MeshIGA; /// allows access of internal data from MeshIGA parent

	int patchID;
	bool type; //< 1D or 2D
	std::vector<double> uKnotVector;
	std::vector<double> vKnotVector;
	int pDegree;
	int qDegree;
	int uNumControlPoints;
	int vNumControlPoints;
	std::vector<int> nodeNet;
	std::vector<double> weights;
};

} /* namespace optclient */
#endif /* MESHIGA_H_ */
