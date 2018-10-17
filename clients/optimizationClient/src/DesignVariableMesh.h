/*
 * DesignInterfaceMesh.h
 *
 *  Created on: May 30, 2014
 *      Author: fabien
 */
/***********************************************************************************************
 * \file DesignVariableMesh.h
 * This file holds the classes DesignVariableMesh
 * \date 5/6/2014
 **************************************************************************************************/
#ifndef DESIGNVARIABLEMESH_H_
#define DESIGNVARIABLEMESH_H_

#include<vector>
#include"MeshInterface.h"
#include"DesignVariableInterface.h"

namespace optclient {

/***********************************************************************************************
 * \brief Class DesignVariableMesh specific design variables related to a standard mesh
***********/
class DesignVariableMesh : public DesignVariableInterface {
	typedef DesignVariableInterface Inherited;
public:
	DesignVariableMesh();
	DesignVariableMesh(MeshInterface* m);
	virtual ~DesignVariableMesh();

	void setMesh(MeshInterface* m){mesh=m;};
	const MeshInterface&  getMesh() {return *mesh;};
	void updateMesh(const std::vector<double>& disp);
private:
	MeshInterface* mesh;

	/**************************************************************************
	 * INHERITED INTERFACE
	 **************************************************************************/
public:
    /***********************************************************************************************
     * \brief update the design variable value on the mesh and proceed to send
     * \author Fabien Pean
     ***********/
	void updateTo();
	void updateTo(const std::vector<double>& data_);
protected:
    /***********************************************************************************************
     * \brief update the design variable from the mesh value
     * \author Fabien Pean
     ***********/
	void updateFrom();


};

} /* namespace optclient */
#endif /* DESIGNINTERFACEMESH_H_ */
