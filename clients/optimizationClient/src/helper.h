/*
 * helper.h
 *
 *  Created on: May 26, 2014
 *      Author: fabien
 */

#ifndef HELPER_H_
#define HELPER_H_
#include<vector>
#include<string>
#include<sstream>

namespace optclient {

namespace helper {

/***********************************************************************************************
 * \brief helper class to convert string into vector of templated type T
 * \param[in] s string input
 * \return vector of desired type T
 * \author Fabien Pean
 ***********/
template<class T>
std::vector<T> str_to_vec(std::string s){
	std::vector<T> out;
	std::stringstream stream(s);
	while(1)
	{
	   T temp;
	   stream >> temp;
	   if(!stream) break; //avoid to take last character which is end termination char
	   out.push_back(temp);
	}
	return out;
};
/***********************************************************************************************
 * \brief helper class to convert string into a value of templated type T
 * \param[in] s string input
 * \return value of desired type T
 * \author Fabien Pean
 ***********/
template<class T>
T str_to_num(std::string s){
	T out;
	std::stringstream stream(s);
	stream >> out;
	return out;
};

void test(std::string s);

}

} /* namespace optclient */
#endif /* HELPER_H_ */
