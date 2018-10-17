/*
 * Sensitivity.cpp
 *
 *  Created on: May 22, 2014
 *      Author: fabien
 */

#include "Sensitivity.h"
#include "Objective.h"
#include "Optimizer.h"
#include "EMPIRE_API.h"
#include "DesignVariableInterface.h"
#include "ProblemData.h"
#include <assert.h>

namespace optclient {

Sensitivity::Sensitivity() :
		designVariables(),
		ref(0.0),
		ref0(0.0) {
}

Sensitivity::~Sensitivity() {
	// TODO Auto-generated destructor stub
}

SensitivityFD::SensitivityFD():
		Inherit(),
		eps(0.05) {
}

SensitivityFD::SensitivityFD(double perturbation):
		Inherit(),
		eps(perturbation) {
}

void SensitivityFD::computeGradient(ObjectiveInterface& objective,std::vector<Constraint*>& constraint,OptimizerInterface& optimizer) {
	perturbationEstimation(objective);

	double refC[constraint.size()];
	for(unsigned int i=0;i<constraint.size();i++) {
		refC[i]=constraint[i]->getConstraint();
	}
	designVariables.front()+=eps;
	optimizer.compute(designVariables);
	objective[0]=(objective.getObjective()-ref) / eps;
	for(unsigned int i=0;i<constraint.size();i++) {
		(*(constraint[i]))[0]=(constraint[i]->getConstraint()-refC[i])/eps;
	}
	for(unsigned int i=1;i<designVariables.size();i++) {
		designVariables[i-1]-=eps;
		designVariables[i]+=eps;
		optimizer.compute(designVariables);
		objective[i]=(objective.getObjective()-ref) / eps;
		for(unsigned int j=0;j<constraint.size();j++) {
			(*(constraint[j]))[0]=(constraint[j]->getConstraint()-refC[j])/eps;
		}
	}
	designVariables.back()-=eps;
}

void SensitivityFD::perturbationEstimation(ObjectiveInterface& objective) {
	ref0=ref;
	ref=objective.getObjective();
	if((ref-ref0)/ref < 1e-2) eps=eps/2;
}

} /* namespace optclient */
