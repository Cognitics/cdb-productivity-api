/*************************************************************************
Copyright (c) 2020 Cognitics, Inc.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
****************************************************************************/
#pragma once


#include "ModelKit/CrossSection.h"
#include <sfa/Matrix.h>
#include <scenegraph/Scene.h>

namespace modelkit
{

	class FlightToCSMImporter
	{
		std::map<std::string,modelkit::Texture *> csmTextureMap;
		scenegraph::MaterialList uniqueMaterials;
		int countPlanePoints(scenegraph::Face &face);
		modelkit::Material *getMaterial(scenegraph::Face &face,modelkit::CrossSection &csm);
		ccl::ObjLog log;
	public:
		FlightToCSMImporter() 
		{
			log.init("FlightToCSMImporter");
			log << ccl::LINFO;
		}
		/**
		 * 

		 **/

		/**
		 * @fn	bool FlightToCSMImporter::ImportFlt(const std::string &filename,
		 *  modelkit::CrossSection &csm, sfa::Matrix &transform);
		 *
		 * @brief	Import a scene to make a cross section  from the edges, faces, textures, and (todo) attachment points
		 *
		 *
		 * @param	filename		 	Filename of the OpenFlight file.
		 * @param [in,out]	csm		 	A reference to a cross section.
		 * @param [in]	transform	A transformation matrix to transform the flight to the required coordinates.
		 *
		 * @return	true if it succeeds, false if it fails.
		 */
		bool ImportFlt(const std::string &filename, modelkit::CrossSection &csm, const sfa::Matrix &transform);

		/**
		 * @fn	bool FlightToCSMImporter::ImportFlt(scenegraph::Scene *scene, modelkit::CrossSection &csm,
		 *  sfa::Matrix &transform);
		 *
		 * @brief	Import OpenFlight Flight file to make a cross section from the edges, faces, textures, and (todo) attachment points

		 *
		 * @param [in]	scene	 	The scene.
		 * @param [in,out]	csm		 	A reference to a cross section.
		 * @param [in]	transform	A transformation matrix to transform the flight to the required coordinates. 
		 * 				The transform is matrix that projects the end of a model on to a 2D plane for the cross section face.
		 *				If the model end faces are aligned along X/Y and have a z value of 0, with z forward, then transform
		 *				will be the identity matrix.
		 *
		 * @return	true if it succeeds, false if it fails.
		 */
		bool ImportFlt(scenegraph::Scene *scene, modelkit::CrossSection &csm, const sfa::Matrix &transform);
		/**
		 * @fn	bool FlightToCSMImporter::ImportFlt(scenegraph::Scene *scene, modelkit::CrossSection &csm,
		 *  sfa::Matrix &transform);
		 *
		 * @brief	Import OpenFlight Flight file to make a cross section from the edges, faces, textures, 
		 * 			and (todo) attachment points. Assumes the model is already transformed.
		 *
		 * @param [in]	scene	 The scene.
		 * @param [in,out]	csm	 A reference to a cross section.
		 *
		 * @return	true if it succeeds, false if it fails.
		 */
		bool ImportFltXZ(const std::string &fltfile, const std::string &csmfile); 
	};
	


}
