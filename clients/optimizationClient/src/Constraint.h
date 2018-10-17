/*
 * Constraint.h
 *
 *  Created on: Jun 17, 2014
 *      Author: fabien
 */
/***********************************************************************************************
 * \file Constraint.h
 * This file holds the classes
 * 		Constraint
 * 		ConstraintVolume
 * \date 5/8/2014
 **************************************************************************************************/
#ifndef CONSTRAINT_H_
#define CONSTRAINT_H_

#include <vector>
#include "MeshInterface.h"
#include <EMPIRE_API.h>
#include <assert.h>
#include <math.h>

namespace optclient {
/********//**
 * \brief Class Constraint base constraint class from which all constraints inherit
 ***********/
class Constraint {
public:
	/*CONSTRUCTOR/DESTRUCTOR*/
	Constraint(char type);
	virtual ~Constraint();
	/*INTERFACE*/
	virtual void computeConstraint()=0;
	virtual void computeGradient()=0;
	/*GETTERS/SETTERS*/
	double getConstraint(){return constraint;};
	const std::vector<double>& getGradient(){return gradient;};

	double& operator[](int i) {return gradient.at(i);};
	void resizeGradient(int i) {gradient.resize(i,0);};

	char typeConstraint(){return type;};

	double getReference(){return refConstraint;};
	void setReference(const double& ref){refConstraint=ref;};

	double getTolerance(){return tolerance;};
	void setTolerance(const double& tol){tolerance=tol;};
protected:
	///type of the constraint < = >
	char type;
	///tolerance to the constraint
	double tolerance;
	///reference constraint value
	double refConstraint;
	///normalized constraint value
	double constraint;
	///gradient of the constraint
	std::vector<double> gradient;
};
/********//**
 * \brief Class ConstraintVolume specific constraint linked to volume/surface to be constrained
 ***********/
class ConstraintVolume : public Constraint {
public:
	ConstraintVolume(char type):Constraint(type),mesh(0){refConstraint=nan("0");};
	virtual ~ConstraintVolume(){};

	void computeConstraint();
	void computeGradient();

	void setMesh(MeshInterface* m){mesh=m;};
	const MeshInterface& getMesh(){return *mesh;};
private:
	///mesh to which volume it is referring to
	MeshInterface* mesh;
};


} /* namespace optclient */
#endif /* CONSTRAINT_H_ */
