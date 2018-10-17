/*
 * MeshInterface.h
 *
 *  Created on: Jun 12, 2014
 *      Author: fabien
 */
/***********************************************************************************************
 * \file MeshInterface.h
 * This file holds the class MeshInterface
 * \date 15/06/2014
 **************************************************************************************************/
#ifndef MESHINTERFACE_H_
#define MESHINTERFACE_H_

#include <vector>

namespace optclient {
/********//**
 * \brief Class MeshInterface for handling mesh data
 ***********/
class MeshInterface {
protected:
	/*CONSTRUCTOR/DESTRUCTOR*/
	MeshInterface();
	virtual ~MeshInterface();

	/*INTERFACE*/
	///read the data from the file provided
	virtual void read()=0;
	///send data to Empire
	virtual void send()=0;
	///display some information about mesh
	virtual void displayInfo()=0;

public:
	///get the coordinates of the nodes of the mesh
	virtual double* getMeshNodes()=0;
	///get number of Nodes in the mesh ( #Dofs = 3*(#Nodes) )
	virtual int getMeshNodeNumber()=0;
	/***********/

	void setDisp(const std::vector<double>& d) {displacement=d;};
	const std::vector<double>& getDisp(){return displacement;};
protected:
	std::vector<double> displacement;
};

} /* namespace optclient */
#endif /* MESHINTERFACE_H_ */
