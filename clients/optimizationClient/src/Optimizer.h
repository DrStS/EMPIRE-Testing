/*
 * Optimizer.h
 *
 *  Created on: May 20, 2014
 *      Author: fabien
 */
/***********************************************************************************************
 * \file Optimizer.h
 * This file holds the class Optimizer
 * \date 5/6/2014
 **************************************************************************************************/
#ifndef OPTIMIZER_H_
#define OPTIMIZER_H_
#include "ObjectiveInterface.h"
#include "Sensitivity.h"
#include "DesignVariableInterface.h"
#include "Constraint.h"
#include <vector>

namespace optclient {

/********//**
 * \brief Class OptimizerInterface common adapter interface for optimization problem
 ***********/
class OptimizerInterface {
public:
    /***********************************************************************************************
     * \brief Constructor
     * \author Fabien Pean
     ***********/
	OptimizerInterface();
    /***********************************************************************************************
     * \brief Constructor
     * \param[in] objective_ objective class used by optimizer
     * \param[in] sensitivty_ sensitivty method used by optimizer
     * \param[in] designVariable_ design variable the optimizer is acting on
     * \author Fabien Pean
     ***********/
	OptimizerInterface(ObjectiveInterface* objective_, DesignVariableInterface* designVariable_);
    /***********************************************************************************************
     * \brief Destructor
     * \author Fabien Pean
     ***********/
	virtual ~OptimizerInterface();

	/**************************************************************************
	 * SETTERS/GETTERS
	 **************************************************************************/
	void setDesignVariables(const std::vector<double>& DVs) {designVariables->setData(DVs);};
	const DesignVariableInterface& getDesignVariables() {return *designVariables;};

	const ObjectiveInterface& getObjective(){return *objective;};
	void setStopCriteria(const std::string& name,const std::string& type,const double value) {stopCriteria.name=name;stopCriteria.type=type;stopCriteria.value=value;};

	void addConstraint(Constraint* c) {constraint.push_back(c);};
	void addConstraint(const std::vector<Constraint*>& c) {constraint=c;};

	void setSensitivity(Sensitivity* s){sensitivity=s;};

	void setAlgorithm(const std::string& algoName) {algorithm=algoName;}

    /***********************************************************************************************
     * \brief minimum actions for an optimization problem. Update mesh, get objective value, send convergence signal
     * \param[in] data special data for mesh update, override DesignVariable data
     * \param[in] convergence state of the convergence signal to send
     * \author Fabien Pean
     ***********/
	void compute(bool convergence=0);
	void compute(const std::vector<double>& data, bool convergence=0);

	void init();
    /***********************************************************************************************
     * \brief setup parameters of the adaptee, to be implemented and defined by refined Optimizer
     * \author Fabien Pean
     ***********/
	virtual void setup(){};
    /***********************************************************************************************
     * \brief call optimization procedure of adaptee, to be implemented and defined by refined Optimizer
     * \author Fabien Pean
     ***********/
	virtual void optimize(){};

protected:
	/// COMPUTATION DATA ///
	///Design variables the optimizer is acting on
	DesignVariableInterface* designVariables;
	///Following the this objective
	ObjectiveInterface* objective;
	///Using gradient information from sensitivity class
	Sensitivity* sensitivity;
	///Storing min or max value of objective in minObjective
	double minObjective;
	///Stores constraint objects desired for problem
	std::vector<Constraint*> constraint;

	/// PARAMETERS DATA ///
	///algorithm specified in xml client file, converted in real algorithm by child class
	std::string algorithm;
	///structure defining the stop criteria, applied by child class
	struct StopCriteria{
		std::string name;
		std::string type;
		double value;
	} stopCriteria;
};



} /* namespace optclient */

#endif /* OPTIMIZER_H_ */
