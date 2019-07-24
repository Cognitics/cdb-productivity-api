#pragma once

#include <ctl/DelaunayTriangulation.h>

namespace scenegraph
{
	bool buildObjFromScene(const std::string outputName, ctl::DelaunayTriangulation *dt, double lWidth, double lNorth, double width, double height);
}