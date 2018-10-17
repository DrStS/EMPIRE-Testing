/*
 * OptimizerNLOpt.h
 *
 *  Created on: Jun 3, 2014
 *      Author: fabien
 */
/***********************************************************************************************
 * \file OptimizerNLOpt.h
 * This file holds the class OptimizerNLOpt ConstraintNLOpt
 * \date 5/6/2014
 **************************************************************************************************/
#ifndef OPTIMIZERNLOPT_H_
#define OPTIMIZERNLOPT_H_

#include <nlopt.hpp>
#include "Optimizer.h"
#include "Constraint.h"
#include <map>

namespace optclient {

/********//**
 * \brief Class OptimizerNLOpt adapter for NLOpt library from MIT
 ***********/
class OptimizerNLOpt : public OptimizerInterface {
	typedef OptimizerInterface Inherit;
public:
	/*CONSTRUCTOR/DESTRUCTOR*/
	OptimizerNLOpt(){};
	OptimizerNLOpt(ObjectiveInterface* objective_, DesignVariableInterface* designVariable_);
	virtual ~OptimizerNLOpt(){};

    /***********************************************************************************************
     * \brief Function called by NLOpt at every timestep, to compute objective and gradient
     * \param[in] x new design variable vector computed by NLOpt
     * \param[out] grad container to store gradient out of sensitivity analysis
     * \author Fabien Pean
     ***********/
	double operator() (const std::vector<double> &x, std::vector<double> &grad);
    /***********************************************************************************************
     * \brief Hack to force NLOpt to use function object
     * \param[in] x new design variable vector computed by NLOpt
     * \param[out] grad container to store gradient out of sensitivity analysis
     * \param[in] data pointer toward structure containing information about problem ("this" pointer)
     * \author Fabien Pean
     ***********/
	static double wrap(const std::vector<double> &x, std::vector<double> &grad, void *data) {
	    return (*reinterpret_cast<OptimizerNLOpt*>(data))(x, grad); }

	class ConstraintNLOpt;
private:
	///adaptee, real class doing the optimization in NLOpt
	nlopt::opt opt;
	std::vector<ConstraintNLOpt> constraintNlopt;
	///mapping from string algorithm to real structure in NLOpt to define them (here an enumeration)
	static const std::map<std::string,nlopt::algorithm> algorithms_map;
        static const std::map<std::string,nlopt::algorithm> create_algo_map();
	/**************************************************************************
	 * INHERITED INTERFACE
	 **************************************************************************/
public:
	void setup();
	void optimize();
};

/********//**
 * \brief Class ConstraintNLOpt adapter for Constraint class suitable for NLOpt library
 ***********/
class OptimizerNLOpt::ConstraintNLOpt {
public:
	/*CONSTRUCTOR/DESTRUCTOR*/
	ConstraintNLOpt(Constraint* c):constraint(c){};
	virtual ~ConstraintNLOpt(){};

    /***********************************************************************************************
     * \brief Function called by NLOpt at every timestep, to retrieve constraint and gradient
     * \param[in] x new design variable vector computed by NLOpt
     * \param[out] grad container to store gradient out of sensitivity analysis
     * \return value of the constraint
     * \author Fabien Pean
     ***********/
	double computeConstraint(const std::vector<double> &x, std::vector<double> &grad) {
		if(!grad.empty())
			grad = constraint->getGradient();
		return constraint->getConstraint();
	};
    /***********************************************************************************************
     * \brief Hack to force NLOpt to use class method
     * \param[in] x new design variable vector computed by NLOpt
     * \param[out] grad container to store gradient out of sensitivity analysis
     * \param[in] data pointer toward structure containing information about problem ("this" pointer)
     * \author Fabien Pean
     ***********/
	static double wrap(const std::vector<double> &x, std::vector<double> &grad, void *data) {
	    return (reinterpret_cast<ConstraintNLOpt*>(data))->computeConstraint(x, grad); };
private:
	Constraint* constraint;
};

} /* namespace optclient */
#endif /* OPTIMIZERNLOPT_H_ */
