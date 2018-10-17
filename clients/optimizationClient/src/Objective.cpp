/*
 * Objective.cpp
 *
 *  Created on: May 20, 2014
 *      Author: fabien
 */

#include "Objective.h"
#include "EMPIRE_API.h"
#include<iostream>
namespace optclient {

void Objective::computeObjective() {
	double temp[size];
	EMPIRE_API_recvSignal_double(const_cast<char*>(name.c_str()), size, temp);
	objectiveValue=temp[1];
}

void ObjectiveLift::computeObjective() {
	double temp[3];
	EMPIRE_API_recvSignal_double("liftDragForce", 3, temp);
	objectiveValue=temp[1];
}

void ObjectiveDrag::computeObjective() {
	double temp[3];
	EMPIRE_API_recvSignal_double("liftDragForce", 3, temp);
	objectiveValue=temp[0];
}

void ObjectiveLiftOverDrag::computeObjective() {
	double temp[3];
	EMPIRE_API_recvSignal_double("liftDragForce", 3, temp);
	objectiveValue=temp[1]/temp[0];
}

void ObjectiveStrain::computeObjective() {
	EMPIRE_API_recvSignal_double("energyStrain", 1, &objectiveValue);

}

} /* namespace optclient */
