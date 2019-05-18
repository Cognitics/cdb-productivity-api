/****************************************************************************
Copyright (c) 2019 Cognitics, Inc.

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

#include "scenegraph/TerrainCullingVisitor.h"

namespace scenegraph
{
    TerrainCullingVisitor::~TerrainCullingVisitor(void)
    {
    }

    TerrainCullingVisitor::TerrainCullingVisitor(const std::vector<sfa::Polygon> &polygons) : polygons(polygons)
    {
    }

    void TerrainCullingVisitor::visiting(Scene *scene)
    {
        for(FaceList::iterator it = scene->faces.begin(), end = scene->faces.end(); it != end; ++it)
        {
            Face &face = *it;
            if(face.verts.empty())
                continue;
            sfa::Polygon terrainPolygon = face.getPolygon();
            for(size_t i = 0, c = polygons.size(); i < c; ++i)
            {
                if(polygons[i].contains(&terrainPolygon))
                {
                    it = scene->faces.erase(it);
                    break;
                }
            }
        }

        traverse(scene);
    }

    void TerrainCullingVisitor::setPolygons(const std::vector<sfa::Polygon> &polygons)
    {
        this->polygons = polygons;
    }


}