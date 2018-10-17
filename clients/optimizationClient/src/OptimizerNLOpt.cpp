/*
 * OptimizerNLOpt.cpp
 *
 *  Created on: Jun 3, 2014
 *      Author: fabien
 */
#include <assert.h>
#include "OptimizerNLOpt.h"
#include <iostream>

namespace optclient {
const std::map<std::string,nlopt::algorithm> OptimizerNLOpt::create_algo_map() {
        std::map<std::string,nlopt::algorithm> tmp;
        tmp[    "GN_DIRECT"]=                      nlopt::GN_DIRECT;
        tmp[    "GN_DIRECT_L"]=                     nlopt::GN_DIRECT_L;
        tmp[    "GN_DIRECT_L_RAND"]=        nlopt::GN_DIRECT_L_RAND;
        tmp[    "GN_DIRECT_NOSCAL"]=        nlopt::GN_DIRECT_NOSCAL;
        tmp[    "GN_DIRECT_L_NOSCAL"]=      nlopt::GN_DIRECT_L_NOSCAL;
        tmp[    "GN_DIRECT_L_RAND_NOSCAL"]= nlopt::GN_DIRECT_L_RAND_NOSCAL;
        tmp[    "GN_ORIG_DIRECT"]=                  nlopt::GN_ORIG_DIRECT;
        tmp[    "GN_ORIG_DIRECT_L"]=        nlopt::GN_ORIG_DIRECT_L;
        tmp[    "GD_STOGO"]=                        nlopt::GD_STOGO;
        tmp[    "GD_STOGO_RAND"]=                   nlopt::GD_STOGO_RAND;
        tmp[    "LD_LBFGS_NOCEDAL"]=        nlopt::LD_LBFGS_NOCEDAL;
        tmp[    "LD_LBFGS"]=                        nlopt::LD_LBFGS;
        tmp[    "LN_PRAXIS"]=                       nlopt::LN_PRAXIS;
        tmp[    "LD_VAR1"]=                         nlopt::LD_VAR1;
        tmp[    "LD_VAR2"]=                         nlopt::LD_VAR2;
        tmp[    "LD_TNEWTON"]=                      nlopt::LD_TNEWTON;
        tmp[    "LD_TNEWTON_RESTART"]=      nlopt::LD_TNEWTON_RESTART;
        tmp[    "LD_TNEWTON_PRECOND"]=      nlopt::LD_TNEWTON_PRECOND;
        tmp[    "LD_TNEWTON_PRECOND_RESTART"]=     nlopt::LD_TNEWTON_PRECOND_RESTART;
        tmp[    "GN_CRS2_LM"]=                      nlopt::GN_CRS2_LM;
        tmp[    "GN_MLSL"]=                         nlopt::GN_MLSL;
        tmp[    "GD_MLSL"]=                         nlopt::GD_MLSL;
        tmp[    "GN_MLSL_LDS"]=                     nlopt::GN_MLSL_LDS;
        tmp[    "GD_MLSL_LDS"]=                     nlopt::GD_MLSL_LDS;
        tmp[    "LD_MMA"]=                                  nlopt::LD_MMA;
        tmp[    "LN_COBYLA"]=                       nlopt::LN_COBYLA;
        tmp[    "LN_NEWUOA"]=                       nlopt::LN_NEWUOA;
        tmp[    "LN_NEWUOA_BOUND"]=         nlopt::LN_NEWUOA_BOUND;
        tmp[    "LN_NELDERMEAD"]=                   nlopt::LN_NELDERMEAD;
        tmp[    "LN_SBPLX"]=                        nlopt::LN_SBPLX;
        tmp[    "LN_AUGLAG"]=                       nlopt::LN_AUGLAG;
        tmp[    "LD_AUGLAG"]=                       nlopt::LD_AUGLAG;
        tmp[    "LN_AUGLAG_EQ"]=                    nlopt::LN_AUGLAG_EQ;
        tmp[    "LD_AUGLAG_EQ"]=                    nlopt::LD_AUGLAG_EQ;
        tmp[    "LN_BOBYQA"]=                       nlopt::LN_BOBYQA;
        tmp[    "GN_ISRES"]=                        nlopt::GN_ISRES;
        tmp[    "AUGLAG"]=                                  nlopt::AUGLAG;
        tmp[    "AUGLAG_EQ"]=                       nlopt::AUGLAG_EQ;
        tmp[    "G_MLSL"]=                                  nlopt::G_MLSL;
        tmp[    "G_MLSL_LDS"]=                      nlopt::G_MLSL_LDS;
        tmp[    "LD_SLSQP"]=                        nlopt::LD_SLSQP;
        tmp[    "LD_CCSAQ"]=                        nlopt::LD_CCSAQ;
        tmp[    "GN_ESCH"]=                         nlopt::GN_ESCH;
        return tmp;
    }
const std::map<std::string,nlopt::algorithm> OptimizerNLOpt::algorithms_map = create_algo_map();

OptimizerNLOpt::OptimizerNLOpt(ObjectiveInterface* objective_, DesignVariableInterface* designVariable_):
		Inherit(objective_,designVariable_) {
}

double OptimizerNLOpt::operator() (const std::vector<double> &x, std::vector<double> &grad) {
	std::cout<<"\t start func call"<<std::endl;
	///update design variables
	setDesignVariables(x);
	///send new mesh displacement and compute values objective/constraints
	compute(0);
	///retrieve value of objective
	double obj=objective->getObjective();
	///update gradient information if algorithm allow it
	if(!grad.empty()) {
		std::cout<<"\t\t start grad call"<<std::endl;
		sensitivity->setDesignVariables(x);
		sensitivity->computeGradient(*objective,constraint,*this);
		grad=objective->getGradient();
		std::cout<<"\t\t end grad call !"<<std::endl;
	}
	std::cout<<"\t end func call !"<<std::endl;
	return obj;
}

void OptimizerNLOpt::setup() {
	///verify that algorithm exists
	assert(algorithms_map.at(algorithm));
	///create optimization object. For a specific algorithm and number of design variable
	opt = nlopt::opt(algorithms_map.at(algorithm), designVariables->size());
	///set parameters of the object required for optimization
	///lower/upper bounds
	opt.set_lower_bounds(designVariables->getLowerBound());
	opt.set_upper_bounds(designVariables->getUpperBound());
	///stop criteria
	if(stopCriteria.name=="f")
		if(stopCriteria.type=="abs") opt.set_ftol_abs(stopCriteria.value);
		else opt.set_ftol_rel(stopCriteria.value);
	else if(stopCriteria.name=="x")
		if(stopCriteria.type=="abs") opt.set_xtol_abs(stopCriteria.value);
		else opt.set_xtol_rel(stopCriteria.value);
	else
		opt.set_xtol_rel(1e-4);
	///max number of evaluation
	opt.set_maxeval(1000);
	///type of the problem min or max
	if(objective->isToMax())
		opt.set_max_objective(OptimizerNLOpt::wrap, this);
	else
		opt.set_min_objective(OptimizerNLOpt::wrap, this);
	///adding constraints
	for(uint i=0;i<constraint.size();i++) {
		constraintNlopt.push_back(ConstraintNLOpt(constraint[i]));
		if(constraint[i]->typeConstraint()=='=')
			opt.add_equality_constraint(OptimizerNLOpt::ConstraintNLOpt::wrap,&constraintNlopt[i],1e-8);
		else
			opt.add_inequality_constraint(OptimizerNLOpt::ConstraintNLOpt::wrap,&constraintNlopt[i],1e-8);
	}

}

void OptimizerNLOpt::optimize() {
	std::cout<<"optClient : start optimizing"<<std::endl;
	std::vector<double> dv = designVariables->getData(); ///need to copy data, otherwise weird results
	assert(dv.empty()!=true);
	nlopt::result result = opt.optimize(dv, minObjective);
	assert(result>=1); /// aka successful opt
	setDesignVariables(dv);
	compute(1);
	std::cout<<"optClient : min/max objective "<<minObjective<<std::endl;
	std::cout<<"optClient : end optimizing !"<<std::endl;
}



} /* namespace optclient */
