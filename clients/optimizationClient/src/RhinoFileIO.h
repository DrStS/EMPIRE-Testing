/*
 * RhinoFileIO.h
 *
 *  Created on: Jun 5, 2014
 *      Author: fabien
 */

#ifndef RHINOFILEIO_H_
#define RHINOFILEIO_H_

#include <string>
#include <iostream>
#include <vector>

namespace optclient {
/***********************************************************************************************
 * \brief Read general data about a Nurbs mesh
 * \param[in] filename name of the file to process
 * \param[out] numPatches the number of patches in the file
 * \param[out] nodesCoordinates the coordinates of all the control points used in the mesh
 * \author Fabien Pean
 ***********/
void readMetaData(const std::string filename, int& numPatches, std::vector<double>& nodesCoordinates);
/***********************************************************************************************
 * \brief Read specific patch from the mesh in filename
 * \param[in] filename name of the file to find the patch in
 * \param[in] patchID identifier of the patch to search
 * \param[out] type Nurbs1D(=0) or Nurbs2D(=1)
 * \param[out] pDegree polynomial degree along first direction
 * \param[out] uKnotVector knot vector along first direction
 * \param[out] qDegree polynomial degree along second direction
 * \param[out] vKnotVector knot vector along second direction
 * \param[out] uNumControlPoints number of control points (CPs) along first direction
 * \param[out] vNumControlPoints number of control points (CPs) along second direction
 * \param[out] nodeNet node identifier of the CPs present in this patch
 * \param[out] weights weights of the CPs in this patch
 * \author Fabien Pean
 ***********/
void readNurbsPatch(const std::string filename,int patchID,bool& type,
		int& pDegree, std::vector<double>& uKnotVector,
		int& qDegree, std::vector<double>& vKnotVector,
		int& uNumControlPoints,
		int& vNumControlPoints,
		std::vector<int>& nodeNet, std::vector<double>& weights);
/***********************************************************************************************
 * \brief Read specific control points information
 * \param[in] filename name of the file to find the patch in
 * \param[in] CPID identifier of the CP data block to look for
 * \param[out] nodeNet node identifier of the CPs present in this patch
 * \param[out] weights weights of the CPs in this patch
 * \author Fabien Pean
 ***********/
void readCP(const std::string filename,int CPID, std::vector<int>& nodeNet, std::vector<double>& weights);


} /* namespace optclient */
#endif /* IGSFILEIO_H_ */
