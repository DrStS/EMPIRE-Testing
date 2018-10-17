/*
 * ProblemOrganizer.cpp
 *
 *  Created on: May 22, 2014
 *      Author: fabien
 */

#include <assert.h>
#include <string>
#include <iostream>
#include <math.h>
#include "EMPIRE_API.h"
#include "ProblemManager.h"
#include "ProblemData.h"
#include "MeshInterface.h"
#include "MeshIGA.h"
#include "Mesh.h"
#include "Objective.h"
#include "Optimizer.h"
#include "OptimizerNLOpt.h"
#include "Constraint.h"
#include "Sensitivity.h"
#include "DesignVariableMesh.h"
#include "helper.h"

namespace optclient {


ProblemManager::ProblemManager(int argc, char *argv[]) {
	assert(argc>=3); // At least the xml file and 1 mesh
	for(int i=1;i<argc;i++)
		argList.push_back(argv[i]);
	assert(argList[0].find(".xml")!=std::string::npos); //check first arg to be the xml
}

ProblemManager::~ProblemManager() {
	// TODO Auto-generated destructor stub
}

void ProblemManager::initialize() {
	EMPIRE_API_Connect(const_cast<char*>(argList[0].c_str()));
	std::cout<<"optClient : connected to Empire !"<<std::endl;

	std::string ID(EMPIRE_API_getUserDefinedText("ID"));
	problemID=(ID!="")?(helper::str_to_num<int>(ID)):(0);

	parser=new ParserInterface();
	parser->commonParse();
}

void ProblemManager::setup() {
	std::cout<<"optClient : start setup"<<std::endl;
	createMeshes();
	createObjects();

	optimizer->init();
	optimizer->setAlgorithm(parser->algorithm);
	optimizer->setStopCriteria(parser->stopCriteria.name,parser->stopCriteria.type,parser->stopCriteria.value);
	optimizer->setup();
	std::cout<<"optClient : setup over !"<<std::endl;
}

void ProblemManager::solve() {
	optimizer->optimize();
}

void ProblemManager::finalize() {
	EMPIRE_API_Disconnect();
}

void ProblemManager::createMeshes() {
	std::cout<<"optClient : create and send mesh"<<std::endl;
	ProblemData& prbData = ProblemData::getInstance(problemID);
	for(unsigned int i=1;i<argList.size();i++) {
		std::cout<<"optClient : argList[i] "<<argList[i]<<std::endl;
		MeshInterface* mesh;
		if(parser->meshType=="IGA")
			mesh=new MeshIGA(argList[i]);
		else
			mesh=new Mesh(argList[i]);
		std::cout<<"optClient : argList[i] "<<argList[i]<<std::endl;
		prbData.addMeshToVec(mesh);
	}
	std::cout<<"optClient : mesh created and sent !"<<std::endl;
}
void ProblemManager::createObjects() {
	std::cout<<"optClient : create local objects"<<std::endl;

	ProblemData& prbData = ProblemData::getInstance(problemID);

	/*****DEAL WITH OBJECTIVE*****/
	ObjectiveInterface* objective;
	if(parser->objective.name=="LiftDrag")
		objective=new ObjectiveLiftOverDrag();
	else if(parser->objective.name=="Lift")
		objective=new ObjectiveLift();
	else if(parser->objective.name=="Drag")
		objective=new ObjectiveDrag();
	else if(parser->objective.name=="Strain")
		objective=new ObjectiveStrain();
	else if(parser->objective.name!=""){
		std::stringstream sstream(parser->objective.name);
		std::string name;sstream>>name;
		int size;sstream>>size;
		objective=new Objective(name,size);
	} else
		objective=NULL;
	assert(objective!=NULL);
	objective->setType(parser->objective.minmax);

	/*****DEAL WITH CONSTRAINT*****/
	//TODO deal with more constraints
	Constraint* constraint=NULL;
	if(!parser->constraint.name.empty()) {
		if(parser->constraint.name[0]=="volume") {
			constraint=new ConstraintVolume(parser->constraint.type[0]);
			///parse info
			std::string tmp;
			std::stringstream sstream(parser->constraint.info[0]);
			double tol = nan("0"), ref = nan("0");
			sstream >> tmp;
			if(!tmp.empty()){
				tol = helper::str_to_num<double>(tmp);
				constraint->setTolerance(tol);
			}
			sstream >> tmp;
			if(!tmp.empty()){
				ref = helper::str_to_num<double>(tmp);
				constraint->setReference(ref);
			}
			//TODO not over here !!!
		} else
			constraint=NULL;
	}
	//assert(constraint!=NULL);

	/*****DEAL WITH SENSITIVITY*****/
	//TODO deal with more sensitivities modules
	Sensitivity* sensitivity=NULL;
	if(parser->sensitivity.name=="default")
		sensitivity=new SensitivityFD();
	else if(parser->sensitivity.name=="FiniteDifference")
		if(parser->sensitivity.info.empty())
			sensitivity=new SensitivityFD();
		else
			sensitivity=new SensitivityFD(helper::str_to_num<double>(parser->sensitivity.info));

	else
		sensitivity=NULL;
	assert(sensitivity!=NULL);

	/*****DEAL WITH DESIGN VAR*****/
	//TODO deal with more meshes
	DesignVariableInterface* designVariables=new DesignVariableMesh(prbData.getMesh(0)); //< get first from first mesh
	std::vector<int> designVariablesIndex=helper::str_to_vec<int>(parser->mapping);
	designVariables->setMapping(designVariablesIndex);
	///assign symmetry map if present
	if(!parser->symmetry.empty()) {
		std::stringstream sstream(parser->symmetry);
		std::string tmp;
		std::map<int, int> symDOF;
		std::vector<double> symFactor;
		std::getline(sstream,tmp,';');
		do {
			std::stringstream sstream2(tmp);
			tmp.clear();
			int in,out;
			sstream2>>in;
			sstream2>>out;
			symDOF.insert(std::make_pair<int,int>(in,out));
			double factor=0;
			sstream2>>factor;
			if(factor)
				symFactor.push_back(factor);
			else
				symFactor.push_back(1.0);
			std::getline(sstream,tmp,';');
		}while(tmp!="");
		assert(symDOF.size()==symFactor.size());
		designVariables->setSymmetry(symDOF);
		designVariables->setFactor(symFactor);
	}
	///assign lower and upper bound
	std::vector<double> lb = helper::str_to_vec<double>(parser->lowerBound);
	std::vector<double> ub = helper::str_to_vec<double>(parser->upperBound);
	assert(!ub.empty() && !lb.empty());
	if(lb.size()==1)
		designVariables->setLowerBound(lb[0]);
	else
		designVariables->setLowerBound(lb);
	if(ub.size()==1)
		designVariables->setUpperBound(ub[0]);
	else
		designVariables->setUpperBound(ub);

	/*****DEAL WITH OPTIMIZER*****/
	optimizer=NULL;
	if(parser->library=="default")
		optimizer=new OptimizerNLOpt(objective,designVariables);
	else if (parser->library=="NLOpt")
		optimizer=new OptimizerNLOpt(objective,designVariables);
	else
		optimizer=NULL;
	assert(optimizer!=NULL);

	///decorate the optimizer
	if(constraint)
		optimizer->addConstraint(constraint);
	optimizer->setSensitivity(sensitivity);

	std::cout<<"optClient : objects created !"<<std::endl;
}

} /* namespace optclient */
