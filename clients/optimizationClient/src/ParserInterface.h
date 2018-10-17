/*
 * ParserInterface.h
 *
 *  Created on: Jun 3, 2014
 *      Author: fabien
 */
/***********************************************************************************************
 * \file ParserInterface.h
 * This file holds the class ParserInterface
 * \date 09/07/2014
 **************************************************************************************************/
#ifndef PARSERINTERFACE_H_
#define PARSERINTERFACE_H_

#include <string>
#include <vector>

namespace optclient {
/********//**
 * \brief Class ParserInterface contains common rules to parse xml client file
 ***********/
class ParserInterface {
public:
	/*CONSTRUCTOR/DESTRUCTOR*/
	ParserInterface();
	virtual ~ParserInterface();

	/***********************************************************************************************
	 * \brief Parse the common properties of the xml file and stores the data
	 * \author Fabien Pean
	 ***********/
	void commonParse();
	/***********************************************************************************************
	 * \brief Parse the specific xml data in case the used library requires special information
	 * \author Fabien Pean
	 ***********/
	virtual void specificParse(){};

public:
	std::string library;
	std::string algorithm;
	struct {
		std::string minmax;
		std::string name;
	} objective;
	struct {
		std::vector<std::string> name;
		std::vector<char> type; //< either < or = or > symbol
		std::vector<std::string> info; //< info of the constraint
	} constraint;
	struct {
		std::string name;
		std::string info;
	}sensitivity;
	struct {
		std::string name;
		std::string type;
		double value;
	} stopCriteria;
	std::string mapping;
	std::string symmetry;
	std::string lowerBound;
	std::string upperBound;
	std::string meshType;
private:
	void parseLibrary();
	void parseAlgorithm();
	void parseMeshInfo();
	void parseObjectiveInfo();
	void parseDesignVariableInfo();
    void parseConstraint();
	void parseSensitivity();
	void parseStopCriteria();
};

} /* namespace optclient */
#endif /* PARSERINTERFACE_H_ */
