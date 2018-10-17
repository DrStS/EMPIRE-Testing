/*
 * Optimizer.cpp
 *
 *  Created on: May 20, 2014
 *      Author: fabien
 */
#include "Optimizer.h"
#include "Sensitivity.h"
#include <assert.h>
#include "EMPIRE_API.h"
#include "ProblemData.h"
#include <iostream>

namespace optclient {

OptimizerInterface::OptimizerInterface():
		designVariables(NULL),
		objective(NULL),
		sensitivity(NULL),
		minObjective(0.0) {
}

OptimizerInterface::OptimizerInterface(ObjectiveInterface* objective_, DesignVariableInterface* designVariable_) :
		designVariables(designVariable_),
		objective(objective_),
		minObjective(0.0) {
}

OptimizerInterface::~OptimizerInterface() {
	if(objective!=NULL)
		delete objective;
	if(designVariables!=NULL)
		delete designVariables;
	if(sensitivity!=NULL)
		delete sensitivity;
	for(unsigned int i=0; i<constraint.size(); i++)
		delete constraint[i];
}

void OptimizerInterface::init(){
	assert(objective!=NULL);
	assert(designVariables!=NULL);
	if(sensitivity==NULL)
		sensitivity=new SensitivityFD();
	objective->resizeGradient(designVariables->size());
	for(unsigned int i=0;i<constraint.size();i++) {
		constraint[i]->resizeGradient(designVariables->size());
	}

}

void OptimizerInterface::compute(bool convergence){
	designVariables->updateTo();
	objective->computeObjective();
	for(unsigned int i=0;i<constraint.size();i++)
		constraint[i]->computeConstraint();
	EMPIRE_API_sendConvergenceSignal(convergence);
}
void OptimizerInterface::compute(const std::vector<double>& data,bool convergence){
	designVariables->updateTo(data);
	objective->computeObjective();
	for(unsigned int i=0;i<constraint.size();i++)
		constraint[i]->computeConstraint();
	EMPIRE_API_sendConvergenceSignal(convergence);
}

} /* namespace optclient */
