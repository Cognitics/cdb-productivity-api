#pragma once

#include "scenegraph/Scene.h"
#include <ctl/DelaunayTriangulation.h>
#include <scenegraph/SceneCropper.h>
#include <scenegraph/FlattenVisitor.h>
#include <scenegraph/TransformVisitor.h>

namespace scenegraph
{
	bool buildObjFromTriangulation(const std::string outputName, double lWidth, double lNorth, double width, double height, int elevWidth, int elevHeight, const ctl::PointList &workingPts);
    Scene *buildSceneFromOBJ(const std::string &filename, bool setTexturePath);
	bool buildObjFromScene(const std::string outputName, double lWest, double lNorth, double width, double height, scenegraph::Scene *inscene);
    bool buildObjXZY(const std::string outputName, double lWest, double lNorth, double width, double height, scenegraph::Scene *inscene, bool xzy);
    void CreateMetaForObj(std::string filename);
	//bool buildObjFromTriangulation(const std::string outputName, std::string imgName, double lWidth, double lNorth, double width, double height, int elevWidth, int elevHeight, const ctl::PointList &workingPts);
}