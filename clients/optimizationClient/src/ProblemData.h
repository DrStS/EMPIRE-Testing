/*
 * ProblemData.h
 *
 *  Created on: May 22, 2014
 *      Author: fabien
 */

/***********************************************************************************************
 * \file ProblemData.h
 * This file holds the class ProblemData
 * \date 5/8/2014
 **************************************************************************************************/
#ifndef PROBLEMDATA_H_
#define PROBLEMDATA_H_
#include<map>
#include<vector>
#include<string>
#include"MeshInterface.h"
#include"DesignVariableInterface.h"

namespace optclient {
/********//**
 * \brief Class ProblemData stores pointer of main structures for every running optimization client
 ***********/
class ProblemData {
public:
	/***********************************************************************************************
	 * \brief Return the ProblemData pointer of the desired problem id
	 * \param[in] id of the optimization problem
	 * \return reference to the problem data class
	 * \author Fabien Pean
	 ***********/
	static ProblemData& getInstance(unsigned int i = 0)
	{
		if(i>=instances.size()) {
			instances.resize(i+1,NULL);
			instances[i]=new ProblemData();
		}
		return *(instances[i]);
	}
private:
	static std::vector<ProblemData*> instances;

public:
	virtual ~ProblemData();
private:
	ProblemData();
	ProblemData(const ProblemData&){};
	ProblemData& operator=(const ProblemData&);

public:
	/**************************************************************************
	 * SETTERS/GETTERS
	 **************************************************************************/
	void addMeshToVec(MeshInterface*);
	void addDVsToVec(DesignVariableInterface*);
	void addMeshToMap(std::string&, MeshInterface*);
	void addDVsToMap(MeshInterface*,DesignVariableInterface* );

	MeshInterface* getMesh(int i = 0);
	DesignVariableInterface* getDV(int i = 0);
	/**************************************************************************/

private:
	///vector of the interface to the meshes
	std::vector<MeshInterface*> meshesVec;
	///vector of the interfaces to design variable
	std::vector<DesignVariableInterface*> designVariablesVec;
	///map referring name to meshes
	std::map<std::string,MeshInterface*> meshesMap;
	///map referring mesh to the design variables they are linked to
	std::map<MeshInterface*,DesignVariableInterface* > designVariablesMap;

};

} /* namespace optclient */
#endif /* PROBLEMDATA_H_ */

