/*
 * DesignInterfaceMesh.cpp
 *
 *  Created on: May 30, 2014
 *      Author: fabien
 */

#include "MeshInterface.h"
#include "DesignVariableMesh.h"
#include "EMPIRE_API.h"
#include <iostream>

namespace optclient {

DesignVariableMesh::DesignVariableMesh() : Inherited(),mesh(NULL) {
}

DesignVariableMesh::DesignVariableMesh(MeshInterface* m) : Inherited(),mesh(m) {
}

DesignVariableMesh::~DesignVariableMesh() {
}

void DesignVariableMesh::updateMesh(const std::vector<double>& disp) {
	mesh->setDisp(disp);
}

void DesignVariableMesh::updateFrom(){
	data.resize(mapping.size(),0);
}

void DesignVariableMesh::updateTo(){
	const int meshNodeNumber=mesh->getMeshNodeNumber();
	std::vector<double> tmp(meshNodeNumber*3,0);
	int i;
	i=0;
	for(std::vector<int>::iterator it = mapping.begin(); it != mapping.end(); ++it) {
		tmp[*it-1]=data[i];//TODO here hack for working with array starting with 1 (matlab) and not 0 (C-style)
		i++;
	}
	i=0;
        for(std::map<int,int>::iterator it = symmetry.begin(); it != symmetry.end(); ++it) {
		tmp[it->first-1]=factor[i]*tmp[it->second-1];//TODO here hack for working with array starting with 1 (matlab) and not 0 (C-style)
		i++;
	}
	EMPIRE_API_sendDataField("", meshNodeNumber*3, tmp.data());
	mesh->setDisp(tmp);
}

void DesignVariableMesh::updateTo(const std::vector<double>& data_){
	const int meshNodeNumber=mesh->getMeshNodeNumber();
	std::vector<double> tmp(meshNodeNumber*3,0);
	int i;
	i=0;
        for(std::vector<int>::iterator it = mapping.begin(); it != mapping.end(); ++it) {
		tmp[*it-1]=data_[i];//TODO here hack for working with array starting with 1 (matlab) and not 0 (C-style)
		i++;
	}
	i=0;
        for(std::map<int,int>::iterator it = symmetry.begin(); it != symmetry.end(); ++it) {
		tmp[it->first-1]=factor[i]*tmp[it->second-1];//TODO here hack for working with array starting with 1 (matlab) and not 0 (C-style)
		i++;
	}
	EMPIRE_API_sendDataField("", meshNodeNumber*3, tmp.data());
	mesh->setDisp(tmp);
}

} /* namespace optclient */
