/*
 * Sensitivity.h
 *
 *  Created on: May 22, 2014
 *      Author: fabien
 */
/***********************************************************************************************
 * \file Sensitivity.h
 * This file holds the classes Sensitivity and SensitivityFD
 * \date 5/6/2014
 **************************************************************************************************/
#ifndef SENSITIVITY_H_
#define SENSITIVITY_H_

#include <vector>

namespace optclient {

/*FORWARD DECLARATION*/
class OptimizerInterface;
class ObjectiveInterface;
class Constraint;

/********//**
 * \brief Class Sensitivity interface
 ***********/
class Sensitivity {
public:
	/*CONSTRUCTOR/DESTRUCTOR*/
	Sensitivity();
	virtual ~Sensitivity();
    /***********************************************************************************************
     * \brief computeGradient virtual class to let handle real process by child class
     * \param[in] objective for which objective to compute the gradient
     * \param[in] constraint list of constraint to compute their jacobian
     * \param[in] optimizer using optimizer information method to process gradient
     * \author Fabien Pean
     ***********/
	virtual void computeGradient(ObjectiveInterface& objective, std::vector<Constraint*>& constraint, OptimizerInterface& optimizer){};

	/**************************************************************************
	 * SETTERS/GETTERS
	 **************************************************************************/
	void setDesignVariables(const std::vector<double>& DVs) {designVariables=DVs;};

protected:
	///design variables values stored locally
	std::vector<double> designVariables;
	///current value of objective (at t[i])
	double ref;
	///previous value of objective (at t[i-1]) for perturbation estimation
	double ref0;
};

/********//**
 * \brief Class Sensitivity with finite difference
 ***********/
class SensitivityFD : public Sensitivity {
	typedef Sensitivity Inherit;
public:
	SensitivityFD();
	SensitivityFD(double perturbation);
	virtual ~SensitivityFD(){};

    /***********************************************************************************************
     * \brief perturbationEstimation to compute manually the perturbation to apply
     * \param[in] objective for which objective to compute the perturbation
     * \author Fabien Pean
     * \TODO improve this method according to state of the art ?
     ***********/
	void perturbationEstimation(ObjectiveInterface& objective);
	void setPerturbation(double eps_){eps=eps_;};
private:
	///value of the perturbation for finite difference evaluation
	double eps;

	/**************************************************************************
	 * INHERITED INTERFACE
	 **************************************************************************/
public:
	void computeGradient(ObjectiveInterface& objective, std::vector<Constraint*>& constraint, OptimizerInterface& optimizer);
};

} /* namespace optclient */
#endif /* SENSITIVITY_H_ */
