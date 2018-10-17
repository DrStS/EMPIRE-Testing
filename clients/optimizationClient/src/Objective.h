/*
 * Objective.h
 *
 *  Created on: May 20, 2014
 *      Author: fabien
 */
/***********************************************************************************************
 * \file Objective.h
 * This file holds the classes
 * 		Objective
 * 		ObjectiveLift
 * 		ObjectiveDrag
 * 		ObjectiveLiftOverDrag
 * 		ObjectiveStrain
 * \date 5/6/2014
 **************************************************************************************************/
#ifndef OBJECTIVE_H_
#define OBJECTIVE_H_
#include <vector>
#include <string>
#include "ObjectiveInterface.h"

namespace optclient {

/***********************************************************************************************
 * \brief Objective, default objective defined by signal name and size to be received from Empire
***********/
class Objective :  public ObjectiveInterface  {
	typedef ObjectiveInterface Inherit;
public:
	Objective();
	Objective(std::string name_, int size_) : name(name_),size(size_){};
	~Objective(){};

	void computeObjective();
private:
	///name of the signal, objective to receive
	std::string name;
	///size of the signal to receive
	int size;
};

/***********************************************************************************************
 * \brief ObjectiveLift, objective dedicated to receive lift value
***********/
class ObjectiveLift : public ObjectiveInterface {
	typedef ObjectiveInterface Inherit;
public:
	ObjectiveLift(){};
	virtual ~ObjectiveLift(){};

	void computeObjective();
};
/***********************************************************************************************
 * \brief ObjectiveDrag, objective dedicated to return drag value
***********/
class ObjectiveDrag : public ObjectiveInterface {
	typedef ObjectiveInterface Inherit;
public:
	ObjectiveDrag(){};
	virtual ~ObjectiveDrag(){};

	void computeObjective();
};
/***********************************************************************************************
 * \brief ObjectiveLiftOverDrag, objective dedicated to receive lift over drag value
***********/
class ObjectiveLiftOverDrag : public ObjectiveInterface {
	typedef ObjectiveInterface Inherit;
public:
	ObjectiveLiftOverDrag(){};
	virtual ~ObjectiveLiftOverDrag(){};

	void computeObjective();
};
/***********************************************************************************************
 * \brief ObjectiveStrain, objective dedicated to receive strain energy
***********/
class ObjectiveStrain : public ObjectiveInterface {
	typedef ObjectiveInterface Inherit;
public:
	ObjectiveStrain(){};
	virtual ~ObjectiveStrain(){};

	void computeObjective();
};





} /* namespace optclient */
#endif /* OBJECTIVE_H_ */
