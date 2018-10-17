/*
 * Constraint.cpp
 *
 *  Created on: Jun 18, 2014
 *      Author: fabien
 */

#include "Constraint.h"

namespace optclient {

Constraint::Constraint(char type_):
		type(type_),
		tolerance(1e-6),
		refConstraint(nan("0")),
		constraint(),
		gradient() {
	assert((type=='<' || type=='=' || type=='>'));
}

Constraint::~Constraint() {
}

void ConstraintVolume::computeConstraint() {
	EMPIRE_API_recvSignal_double("volume", 1, &constraint);
	if(isnan(refConstraint)) {
		assert(constraint!=0); /// ensure consistency for normalized computation
		refConstraint=constraint;
	}
	assert(refConstraint==refConstraint);
// 	constraint=fabs(refConstraint-constraint);
//         return;
	switch(type) {
	case '<' : constraint=constraint/refConstraint-1;break;
	case '=' : constraint=fabs(constraint/refConstraint-1);break;
	case '>' : constraint=1-constraint/refConstraint;break;
	default : assert(0); break; ///unfeasible case
	}
}

void ConstraintVolume::computeGradient() {
};


} /* namespace optclient */
