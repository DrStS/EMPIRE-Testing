/*
 * ProblemData.cpp
 *
 *  Created on: May 22, 2014
 *      Author: fabien
 */

#include "ProblemData.h"

namespace optclient {

std::vector<ProblemData*> ProblemData::instances;

ProblemData::ProblemData() {
	// TODO Auto-generated constructor stub
}

ProblemData::~ProblemData() {
	if(!instances.empty()) {
            for(std::vector<ProblemData*>::iterator it=instances.begin();it!=instances.end();it++){
			delete *it;
			*it=NULL;
		}
	}
}

void ProblemData::addMeshToVec(MeshInterface* mesh) {
	meshesVec.push_back(mesh);
}

void ProblemData::addDVsToVec(DesignVariableInterface* designVariables) {
	designVariablesVec.push_back(designVariables);
}

void ProblemData::addMeshToMap(std::string& name,MeshInterface* mesh) {
	meshesMap[name]=mesh;
}

void ProblemData::addDVsToMap(MeshInterface* mesh, DesignVariableInterface* DVs) {
	designVariablesMap[mesh]=DVs;
}

MeshInterface* ProblemData::getMesh(int i) { return meshesVec.at(i); }

DesignVariableInterface* ProblemData::getDV(int i) { return designVariablesVec.at(i); }

} /* namespace optclient */
