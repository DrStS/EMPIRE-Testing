/*
 * DesignVariableContainer.h
 *
 *  Created on: May 30, 2014
 *      Author: fabien
 */
/***********************************************************************************************
 * \file DesignVariableInterface.h
 * This file holds the classes DesignVariableInterface
 * \date 5/6/2014
 **************************************************************************************************/
#ifndef DESIGNVARIABLEINTERFACE_H_
#define DESIGNVARIABLEINTERFACE_H_

#include <vector>
#include <map>
#include <assert.h>

namespace optclient {
/***********************************************************************************************
 * \brief DesignVariableInterface, common interface for all design variables types
***********/
class DesignVariableInterface {
public:
	/*CONSTRUCTOR/DESTRUCTOR*/
	DesignVariableInterface();
	virtual ~DesignVariableInterface();

public:
	int size(){return data.size();};

	std::vector<double>& operator() (void) {return data;};
	const std::vector<double>& getData() {return data;};
	void setData(const std::vector<double>& d) {assert(d.size()==data.size());data=d;};

	///assign mapping to external support (e.g mesh)
	void setMapping(const std::vector<int>& m){mapping=m;updateFrom();};
	///assign symmetry properties
	void setSymmetry(const std::map<int,int>& s){symmetry=s;setFactor();};
	void setFactor(const std::vector<double>& f){assert(f.size()==symmetry.size());factor=f;};
	void setFactor(void) {factor.resize(symmetry.size(),1);};
	///assign vector value
	void setLowerBound(const std::vector<double>& lb_) {assert(lb_.size()==data.size());lb=lb_;};
	void setUpperBound(const std::vector<double>& ub_) {assert(ub_.size()==data.size());ub=ub_;};
	///assign same value for every dof
	void setLowerBound(const double lb_) {lb.resize(data.size(),lb_);};
	void setUpperBound(const double ub_) {ub.resize(data.size(),ub_);};
	///get vector value of lb/ub
	const std::vector<double>& getLowerBound() {return lb;};
	const std::vector<double>& getUpperBound() {return ub;};


    /***********************************************************************************************
     * \brief to be implemented by child classes. Process to update them externally (i.e. send them to empire)
     * \param[in] data special data for mesh update, override class DesignVariable member data of this class
     * \author Fabien Pean
     ***********/
	virtual void updateTo(){};
	virtual void updateTo(const std::vector<double>& data_){};
protected:
    /***********************************************************************************************
     * \brief to be implemented by child classes. Process to update them in/ex-ternally (e.g from a mesh)
     * \author Fabien Pean
     ***********/
	virtual void updateFrom(){};

protected:
	///storing real values of design variables
	std::vector<double> data;
	///storing the mapping extern local data (e.g mesh nodes index)
	std::vector<int> mapping;
	///storing the indexes to symmetric dof by comparing it to mapping
	std::map<int,int> symmetry;
	///storing factor by which multiply "symmetric dof"
	std::vector<double> factor;
	///containers for lower and upper bound, required for most of algorithms
	std::vector<double> lb,ub;

};

} /* namespace optclient */
#endif /* DESIGNVARIABLEINTERFACE_H_ */
