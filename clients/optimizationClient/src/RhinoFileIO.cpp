/*
 * RhinoFileIO.cpp
 *
 *  Created on: Jun 5, 2014
 *      Author: fabien
 */

#include "RhinoFileIO.h"
#include "helper.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

namespace optclient {
using namespace std;

void readMetaData(std::string filename, int& numPatches, std::vector<double>& nodesCoordinates) {
	string line;
	ifstream myfile(filename.c_str());
	if (myfile.is_open())
	while (getline(myfile,line))
	{
		  if(line.find("ND-COOR")!=string::npos) {
			  getline(myfile,line);
			  do{ //until no more node data found
				  double node_tmp;
				  stringstream sstream(line);
				  string tmp; //skip word NODE
				  while(tmp!="X") sstream>>tmp; //search for word X
				  sstream>>node_tmp;
				  nodesCoordinates.push_back(node_tmp);
				  while(tmp!="Y")sstream>>tmp; //search for word Y
				  sstream>>node_tmp;
				  nodesCoordinates.push_back(node_tmp);
				  while(tmp!="Z")sstream>>tmp; //search for word Z
				  sstream>>node_tmp;
				  nodesCoordinates.push_back(node_tmp);
				  getline(myfile,line);
			  }while(line.find("NODE")!=std::string::npos);
		  }
		  if(line.find("NURBS_PATCH")!=string::npos) {
			  numPatches++; // count number of line with word NURBS_PATCH found
		  }
	}
	myfile.close();
}

void readNurbsPatch(std::string filename,int patchID, bool& type,
		int& pDegree, std::vector<double>& uKnotVector,
		int& qDegree, std::vector<double>& vKnotVector,
		int& uNumControlPoints,
		int& vNumControlPoints,
		std::vector<int>& nodeNet, std::vector<double>& weights) {
	string line;
	ifstream myfile(filename.c_str());

	int ctrl_id;

	if (myfile.is_open())
	while (getline(myfile,line))
	{
		if(line.find("NURBS_PATCH")!=string::npos) {
			stringstream sstream(line);
			string tmp;

			sstream>>tmp; ///skip word NURSB_PATCH
			/// Check id
			int id;
			sstream >> id;
			if(id != patchID) continue;

			sstream>>tmp; //skip word :
			string typeStr;
			sstream>>typeStr;
			if(typeStr =="NURBS_2D") {
			  type=1;
			  getline(myfile,line);
			  do { //until node patch information found
				  stringstream sstream2(line);
				  sstream2>>tmp; //get first word
				  if(tmp=="CTRL_PTS"){
					  sstream2>>tmp;
					  sstream2>>tmp;
					  sstream2>>ctrl_id;
				  }
				  if(tmp=="NCTRL"){
					  sstream2>>tmp;
					  sstream2>>uNumControlPoints;
					  uNumControlPoints++; //FIXME file value always N-1 CP than reality. Why ?
				  }
				  if(tmp=="MCTRL"){
					  sstream2>>tmp;
					  sstream2>>vNumControlPoints;
					  vNumControlPoints++; //FIXME file value always N-1 CP than reality. Why ?
				  }
				  if(tmp=="PDEG"){
					  sstream2>>tmp;
					  sstream2>>pDegree;
				  }
				  if(tmp=="QDEG"){
					  sstream2>>tmp;
					  sstream2>>qDegree;
				  }
				  if(tmp=="UKNOT"){
					  sstream2>>tmp; //remove "="
					  std::getline(sstream2,tmp,',');
					  do{
						  double knot=helper::str_to_num<double>(tmp);
						  uKnotVector.push_back(knot);
						  tmp.clear();
						  std::getline(sstream2,tmp,',');
					  }while(tmp!="");
				  }
				  if(tmp=="VKNOT"){
					  sstream2>>tmp; //remove "="
					  std::getline(sstream2,tmp,',');
					  do{
						  double knot=helper::str_to_num<double>(tmp);
						  vKnotVector.push_back(knot);
						  tmp.clear();
						  std::getline(sstream2,tmp,',');
					  }while(tmp!="" );
				  }
				  getline(myfile,line);
			  } while(line.find("UKNOT")!=string::npos || line.find("VKNOT")!=string::npos || line.find("PDEG")!=string::npos || line.find("QDEG")!=string::npos || line.find("MCTRL")!=string::npos || line.find("NCTRL")!=string::npos || line.find("CTRL_PTS")!=string::npos);
			  break;
			}
		}
	}
	myfile.close();

//	cout<<"PDEG "<<pDegree<<"/ QDEG"<<qDegree<<endl;
//	cout<<"NCTRL "<<uNumControlPoints<<"/ MCTRL"<<vNumControlPoints<<endl;
//	cout<<"VKNOT "<<vKnotVector.size()<<"/ UKNOT"<<uKnotVector.size()<<endl;

	/// read controls points information related to this patch
	readCP(filename,ctrl_id,nodeNet,weights);

//	cout<<"WEIGHT "<<weights.size()<<endl;
//	cout<<"nodeNet "<<nodeNet.size()<<endl;
}

void readCP(std::string filename,int cpID, std::vector<int>& nodeNet, std::vector<double>& weights) {
	string line;
	ifstream myfile(filename.c_str());
	if (myfile.is_open())
	while (getline(myfile,line))
	{
		if(line.find("CTRL_PTS_NODES")!=string::npos) {
			int ctrl_id;
			string tmp;
			int tmp_node_id;
			double tmp_weight;
			stringstream sstream(line);
			/// Check id
			sstream>>tmp; //skip word CTRL_PTS_NODES
			sstream>>ctrl_id;
			if(ctrl_id != cpID) continue;

			getline(myfile,line);
			do {
			  stringstream sstream2(line);
			  sstream2>>tmp; //skip word NODE_ID
			  sstream2>>tmp_node_id;
			  nodeNet.push_back(tmp_node_id-1); //store C-style node reference
			  sstream2>>tmp; //skip word W
			  sstream2>>tmp_weight;
			  weights.push_back(tmp_weight);
			  getline(myfile,line);
			}while(line.find("NODE_ID")!=string::npos);
		}
	}
	myfile.close();
}

} /* namespace optclient */
