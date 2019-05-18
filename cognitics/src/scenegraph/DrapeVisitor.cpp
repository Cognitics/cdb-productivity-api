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

#include "scenegraph/DrapeVisitor.h"

namespace scenegraph
{
    DrapeVisitor::~DrapeVisitor(void)
    {
    }

    DrapeVisitor::DrapeVisitor(Scene *terrainScene) : terrainScene(terrainScene)
    {
    }

    void DrapeVisitor::visiting(Scene *scene)
    {
        if(!terrainScene)
            return;

        for(FaceList::iterator terrainIT = terrainScene->faces.begin(), terrainEND = terrainScene->faces.end(); terrainIT != terrainEND; ++terrainIT)
        {
            Face &terrainFace = *terrainIT;
            if(terrainFace.verts.empty())
                continue;
            sfa::Polygon terrainPolygon = terrainFace.getPolygon();

            for(FaceList::iterator it = scene->faces.begin(), end = scene->faces.end(); it != end; ++it)
            {
                Face &face = *it;
                for(size_t i = 0, c = face.verts.size(); i < c; ++i)
                {
                    sfa::Point &point = face.verts.at(i);
                    if(terrainPolygon.intersects(&point))
                    {
                        terrainFace.interpolatePointInFace(point);
                        point.setZ(point.Z() + 0.5f);
                    }
                }
            }
        }

        traverse(scene);
    }

    void DrapeVisitor::setTerrainScene(Scene *scene)
    {
        this->terrainScene = terrainScene;
    }


}