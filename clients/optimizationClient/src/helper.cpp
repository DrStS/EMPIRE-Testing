/*
 * helper.cpp
 *
 *  Created on: May 26, 2014
 *      Author: fabien
 */

#include "helper.h"

#include "MeshIGA.h"
#include<EMPIRE_API.h>
#include<iostream>

namespace optclient {

namespace helper {
template<> std::vector<double> str_to_vec(std::string);
template<> std::vector<int> str_to_vec(std::string);

template<> int str_to_num(std::string);
template<> double str_to_num(std::string);

void test(std::string s) {
	EMPIRE_API_Connect("optClient.xml");
	MeshIGA m(s);

	EMPIRE_API_Disconnect();

	std::cout<<"Done !"<<std::endl;
}

}

} /* namespace optclient */
