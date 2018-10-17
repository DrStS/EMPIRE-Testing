/*
 * ProblemOrganizer.h
 *
 *  Created on: May 22, 2014
 *      Author: fabien
 */
/***********************************************************************************************
 * \file ProblemManager.h
 * This file holds the class ProblemManager
 * \date 5/8/2014
 **************************************************************************************************/
#ifndef PROBLEMMANAGER_H_
#define PROBLEMMANAGER_H_
#include <vector>
#include <string>
#include <sstream>
#include "Optimizer.h"
#include "ParserInterface.h"

namespace optclient {
/********//**
 * \brief Class ProblemManager driving the optimization process
 ***********/
class ProblemManager {
public:
	/*CONSTRUCTOR/DESTRUCTOR*/
	ProblemManager(int argc, char *argv[]);
	virtual ~ProblemManager();

    /***********************************************************************************************
     * \brief Function called first to setup important information / connection to Empire
     * \author Fabien Pean
     ***********/
	void initialize();
    /***********************************************************************************************
     * \brief Function following initialization preparing all relevant data for this client
     * \author Fabien Pean
     ***********/
	void setup();
    /***********************************************************************************************
     * \brief Function called to solve the optimization problem specified
     * \author Fabien Pean
     ***********/
	void solve();
    /***********************************************************************************************
     * \brief Function called at the end to delete data, connection termination
     * \author Fabien Pean
     ***********/
	void finalize();

private:
    /***********************************************************************************************
     * \brief Create the mesh object according to the argument given to the program
     * \author Fabien Pean
     ***********/
	void createMeshes();
    /***********************************************************************************************
     * \brief Poor Factory for creating all object according to xml file
     * \author Fabien Pean
     ***********/
	void createObjects();

	///argument list given to the program
	std::vector<std::string> argList;
	///main object optimizer
	OptimizerInterface* optimizer;
	///parser to get information from xml
	ParserInterface* parser;

	///identification number of the problem, in case of several optimization client running
	int problemID;
};

} /* namespace optclient */
#endif /* PROBLEMMANAGER_H_ */
