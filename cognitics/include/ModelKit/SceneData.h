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

#include <vector>

#include <scenegraph/UserData.h>
#include <scenegraph/Node.h>
#include <ModelKit/CrossSection.h>
#include <ModelKit/ModelKit.h>
#include <sfa/sfa.h>

namespace modelkit
{
	class AvoidZone
	{
	public:
		sfa::Point origin; // origin of the whole scene this avoid zone came from
		CrossSectionSP crossSection;// the cross section used to generate this model.
		std::vector<sfa::Projection2D> projections;
	};

	// Contains all the information needed to clip a scene
	// In other words, all the projections and cross sections
	// for all the scenes we will clip against.
	class AvoidInfo
	{
	public:
		sfa::Point origin; // origin of the scene being clipped
		std::vector<AvoidZone> avoidZones;
	};

	class FaceData : public scenegraph::UserData
	{


	public:
		virtual ~FaceData() {}

	};

	class SceneData : public scenegraph::UserData
	{

	public:
		SceneData() : feature(NULL), ownerScene(NULL){ };
		virtual ~SceneData() {}

		//Shallow copy
		virtual scenegraph::UserData *Clone()
		{
			SceneData *copy = new SceneData();

			copy->modelKit = this->modelKit;
			copy->crossSection = this->crossSection;
			copy->projections = this->projections;
			copy->feature = this->feature;			
			copy->ownerScene = this->ownerScene;			

			return copy;
		}		
		ModelKitSP modelKit;
		CrossSectionSP crossSection;// the cross section used to generate this model.
		std::vector<sfa::Projection2D> projections;//projections for each cross-section.
		const sfa::Feature* feature;
		scenegraph::Scene *ownerScene;
		modelkit::AvoidInfo avoidZones;

	};


}
