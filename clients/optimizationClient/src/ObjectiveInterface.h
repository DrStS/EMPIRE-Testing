/*
 * ObjectiveInterface.h
 *
 *  Created on: May 20, 2014
 *      Author: fabien
 */
/***********************************************************************************************
 * \file ObjectiveInterface.h
 * This file holds the class ObjectiveInterface
 * \date 30/06/2014
 **************************************************************************************************/
#ifndef OBJECTIVEINTERFACE_H_
#define OBJECTIVEINTERFACE_H_
#include <vector>
#include <string>

namespace optclient {
/***********************************************************************************************
 * \brief ObjectiveInterface, common interface for all objective functions
***********/
class ObjectiveInterface {
public:
	/*CONSTRUCTOR/DESTRUCTOR*/
	ObjectiveInterface() : objectiveValue(){};
	virtual ~ObjectiveInterface() {};

    /***********************************************************************************************
     * \brief to be defined by child class, on how to handle the computation (reception) of objective
     * \author Fabien Pean
     ***********/
	virtual void computeObjective()=0;

    /***********************************************************************************************
     * \brief get the current value of objective stored in this class
     * \author Fabien Pean
     ***********/
	double getObjective() {return objectiveValue;}

	const std::vector<double>& getGradient(){return gradient;};
	double& operator[](int i) {return gradient.at(i);};
	void resizeGradient(int i) {gradient.resize(i,0);};
	void setType(const std::string& type) { if(type=="max") minmax=true; else minmax=false;};
	bool isToMax() {return minmax;};
protected:
	///function to minimize (0) or maximize(1)
	bool minmax;
	///value of the objective
	double objectiveValue;
	///gradient of objective
	std::vector<double> gradient;
};

} /* namespace optclient */
#endif /* OBJECTIVEINTERFACE_H_ */
