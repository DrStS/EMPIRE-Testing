/*
 * ParserInterface.cpp
 *
 *  Created on: Jun 3, 2014
 *      Author: fabien
 */

#include "ParserInterface.h"
#include "EMPIRE_API.h"
#include <assert.h>
#include <sstream>
#include "helper.h"

namespace optclient {

ParserInterface::ParserInterface() {
	// TODO Auto-generated constructor stub
}

ParserInterface::~ParserInterface() {
	// TODO Auto-generated destructor stub
}

void ParserInterface::commonParse() {
	parseMeshInfo();
	parseLibrary();
	parseObjectiveInfo();
	parseConstraint();
	parseSensitivity();
	parseAlgorithm();
	parseStopCriteria();
	parseDesignVariableInfo();
}

void ParserInterface::parseLibrary() {
 	library=EMPIRE_API_getUserDefinedText("Library");
	if(library.empty()) library="default";
}
void ParserInterface::parseAlgorithm() {
	algorithm=EMPIRE_API_getUserDefinedText("Algorithm");
	assert(!algorithm.empty());
}
void ParserInterface::parseMeshInfo() {
    meshType=EMPIRE_API_getUserDefinedText("MeshType");
    if(meshType.empty()) meshType="default";
}
void ParserInterface::parseDesignVariableInfo() {
	mapping=EMPIRE_API_getUserDefinedText("DesignVariableIndex");
	lowerBound=EMPIRE_API_getUserDefinedText("LowerBound");
	upperBound=EMPIRE_API_getUserDefinedText("UpperBound");
	assert(!mapping.empty());
	assert(!lowerBound.empty());
	assert(!upperBound.empty());
	symmetry=EMPIRE_API_getUserDefinedText("Symmetry");
}
void ParserInterface::parseObjectiveInfo() {
	std::string tmp;
	std::string minmax,name;
    tmp=EMPIRE_API_getUserDefinedText("Objective");
    assert(!tmp.empty());
    std::stringstream sstream(tmp);
    sstream >> minmax;
    assert(minmax=="min" || minmax=="max");
    objective.minmax=minmax;
    sstream >> name;
    assert(!name.empty());
    objective.name=name;
}
void ParserInterface::parseSensitivity() {
    std::string tmp;
    tmp=EMPIRE_API_getUserDefinedText("Sensitivity");
    std::stringstream sstream(tmp);
    sstream>>sensitivity.name;
    sstream>>sensitivity.info;
    if(sensitivity.name.empty()) sensitivity.name="default";
}
void ParserInterface::parseConstraint() {
    std::string tmp;
	for(int i=1;;i++) {
            std::stringstream tostring;
            tostring << i;
		tmp="Constraint"+tostring.str();
		tmp=EMPIRE_API_getUserDefinedText(const_cast<char*>(tmp.c_str()));
		if(tmp.empty()) break;
	    std::stringstream sstream(tmp);
	    std::string tmp2;
	    sstream >> tmp2;
		constraint.name.push_back(tmp2);
	    sstream >> tmp2;
	    constraint.type.push_back(*(tmp2.c_str()));
	    sstream >> tmp2;
	    constraint.info.push_back(tmp2);
	}
}
void ParserInterface::parseStopCriteria() {
    std::stringstream sstream;
    sstream.str(EMPIRE_API_getUserDefinedText("StopCriteria"));
    sstream >> stopCriteria.name;
    sstream >> stopCriteria.type;
    std::string stopCriteriaValue;
    sstream >> stopCriteriaValue;
    stopCriteria.value=helper::str_to_num<double>(stopCriteriaValue);
    assert(!stopCriteria.name.empty() && !stopCriteria.type.empty() && !stopCriteriaValue.empty());
}

} /* namespace optclient */
                                               
